#include "DnsServiceSimplified.h"

using namespace dns;
using namespace omnetpp;
using namespace inet;

Define_Module(DnsServiceSimplified);

void DnsServiceSimplified::initialize(int stage)
{
    if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        dnsDatabase = check_and_cast<DnsDb *>(findModuleByPath(par("dbPath")));
        authorityMatcher.setPattern(par("domainAuthority"), true, false, false);
    }
    ApplicationBase::initialize(stage);
}

void DnsServiceSimplified::finish()
{
    // Reset the records for the next simulation
}

void DnsServiceSimplified::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(DNS_PORT);
}

void DnsServiceSimplified::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // See if it is a DNS request
    ChunkQueue queue("Incoming data", packet->peekData());

    while (queue.has<DnsRequest>())
    {
        auto request = queue.pop<const DnsRequest>();
        if (request->getOperationCode() == QUERY)
            handleQuery(packet, request.get());
        else
            handleNotImplemented(packet);
    }

    // Release the memory
    delete packet;
}

void DnsServiceSimplified::sendResponseTo(const Packet *packet, DnsRequest *response)
{
    // Retrieve IP:Port from sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int remotePort = packet->getTag<L4PortInd>()->getSrcPort();

    EV_DEBUG << "Sending response to: " << remoteAddress << "\n";

    // Process and package response
    auto responsePacket = new Packet("DnsRequest", Ptr<DnsRequest>(response));
    socket.sendTo(responsePacket, remoteAddress, remotePort);
}

void DnsServiceSimplified::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << "\n";
    delete indication;
}

void DnsServiceSimplified::handleQuery(const Packet *packet, const DnsRequest *request)
{
    EV_DEBUG << "Handling query" << "\n";
    auto questionCount = request->getQuestionArraySize();

    // If there are no questions --> Error
    if (questionCount == 0)
    {
        EV_INFO << "Query with 0 questions" << endl;
        auto response = request->dup();
        response->setReturnCode(ReturnCode::FORMERR);
        sendResponseTo(packet, response);
        return;
    }

    auto allocateIfNull = [this](DnsRequest *p, const DnsRequest *req) -> DnsRequest *
    { if (!p) return prepareResponse(req); else return p; };

    DnsRequest *ok{};
    DnsRequest *error{};

    for (int i = 0; i < questionCount; i++)
    {
        EV_DEBUG << "Processing query" << i << "\n";
        auto records = dnsDatabase->searchRecords(request->getQuestion(i));

        if (records)
        {
            ok = allocateIfNull(ok, request);
            ok->appendQuestion(request->getQuestion(i));
            processRecords(ok, records);
        }
        else
        {
            error = allocateIfNull(error, request);
            error->appendQuestion(request->getQuestion(i));
        }
    }

    // Send the responses
    if (ok)
    {
        ok->setReturnCode(ReturnCode::NOERROR);
        sendResponseTo(packet, ok);
    }

    if (error)
    {
        error->setReturnCode(ReturnCode::NXDOMAIN);
        sendResponseTo(packet, error);
    }
}

void DnsServiceSimplified::processRecords(DnsRequest *response, const std::vector<dns::ResourceRecord *> *records)
{
    for (const auto record : *records)
    {
        if (authorityMatcher.matches(record->getDomain()))
            response->appendAuthoritativeAnswer(*record);
        else
            response->appendNonAuthoritativeAnswer(*record);
    }
}

DnsRequest *DnsServiceSimplified::prepareResponse(const DnsRequest *request)
{
    auto response = new DnsRequest();
    response->setOperationCode(request->getOperationCode());
    response->setRequestId(request->getRequestId());
    response->setIsAuthoritative(true);
    return response;
}

void DnsServiceSimplified::handleNotImplemented(const Packet *packet)
{
    // Select the not implemented response
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    auto response = request->dup();
    response->setReturnCode(ReturnCode::NOTIMP);
    sendResponseTo(packet, response);
}

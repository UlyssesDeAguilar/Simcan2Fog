#include "DnsServiceSimplified.h"

using namespace dns;

Define_Module(DnsServiceSimplified);

void DnsServiceSimplified::initialize(int stage)
{
    switch (stage)
    {
    case INITSTAGE_APPLICATION_LAYER:
        cache = check_and_cast<DnsCache*>(getModuleByPath("^.cache"));
        break;
    };
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
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    switch (request->getOperationCode())
    {
    case QUERY:
        handleQuery(packet);
        break;

    default:
        handleNotImplemented(packet);
        break;
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
    delete indication;
}

void DnsServiceSimplified::handleQuery(const Packet *packet)
{
    EV_DEBUG << "Handling query" << "\n";

    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
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
        auto record = processQuestion(request->getQuestion(i));

        if (record)
        {
            ok = allocateIfNull(ok, request.get());
            ok->insertQuestion(request->getQuestion(i));
            ok->insertAuthoritativeAnswers(*record);
        }
        else
        {
            error = allocateIfNull(error, request.get());
            error->insertQuestion(request->getQuestion(i));
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

DnsRequest *DnsServiceSimplified::prepareResponse(const DnsRequest *request)
{
    auto response = new DnsRequest();
    response->setOperationCode(request->getOperationCode());
    response->setRequestId(request->getRequestId());
    response->setIsAuthoritative(true);
    return response;
}

const ResourceRecord *DnsServiceSimplified::processQuestion(const char *domain)
{
    
    if (cache->hasDomainCached(domain))
        return &cache->getCachedDomain(domain);
    else
        return nullptr;
}

void DnsServiceSimplified::handleNotImplemented(const Packet *packet)
{
    // Select the not implemented response
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    auto response = request->dup();
    response->setReturnCode(ReturnCode::NOTIMP);
    sendResponseTo(packet, response);
}
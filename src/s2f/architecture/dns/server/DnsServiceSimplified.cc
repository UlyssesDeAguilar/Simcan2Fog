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
        authorityMatcher.setPattern(par("authoritativeDomain"), true, false, false);
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
        auto request = queue.pop<DnsRequest>();
        if (request->getOperationCode() == QUERY)
            handleQuery(packet, request);
        else
            handleNotImplemented(packet, request);
    }

    // Release the memory
    delete packet;
}

void DnsServiceSimplified::sendResponseTo(const Packet *packet, Ptr<DnsRequest> response)
{
    // Retrieve IP:Port from sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int remotePort = packet->getTag<L4PortInd>()->getSrcPort();

    EV_DEBUG << "Sending response to: " << remoteAddress << "\n";

    // Process and package response
    auto responsePacket = new Packet("DnsRequest", response);
    socket.sendTo(responsePacket, remoteAddress, remotePort);
}

void DnsServiceSimplified::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << "\n";
    delete indication;
}

void DnsServiceSimplified::handleQuery(const Packet *packet, Ptr<const DnsRequest> request)
{
    EV_DEBUG << "Handling query" << "\n";
    int questionCount = request->getQuestionArraySize();
    auto response = makeShared<DnsRequest>(*request);

    // If there are no questions or more than the practical ones return an error
    // Meanwhile the RFC 1035 specifies more than one question, in the real world it is either ignored or treated as an error.
    if (questionCount == 0 || questionCount > 1)
    {
        EV_WARN << "Query with unsopported number of questions: FORMERR" << "\n";
        response->setReturnCode(ReturnCode::FORMERR);
        sendResponseTo(packet, response);
        return;
    }

    const char *domain = request->getQuestion(0).getDomain();
    EV_DEBUG << "Processing query: " << domain << "\n";

    auto node = dnsDatabase->searchRecords(request->getQuestion(0));

    if (node)
    {
        processRecords(domain, response, *(node->getRecords()));
        response->setReturnCode(ReturnCode::NOERROR);
    }
    else
    {
        response->setReturnCode(ReturnCode::NXDOMAIN);
    }

    sendResponseTo(packet, response);
}

void DnsServiceSimplified::processRecords(const char *domain, Ptr<DnsRequest> response, const std::set<dns::ResourceRecord> &records)
{
    if (authorityMatcher.matches(domain))
    {
        EV_DEBUG << "This server is authoritative for: " << domain << "\n";
        response->setIsAuthoritative(true);
        response->setAuthoritativeAnswerArraySize(records.size());
        int i = 0;
        for (auto &record : records)
            response->setAuthoritativeAnswer(i++, record);
    }
    else
    {
        EV_DEBUG << "This server is non authoritative for: " << domain << "\n";
        response->setIsAuthoritative(false);
        response->setNonAuthoritativeAnswerArraySize(records.size());
        int i = 0;
        for (auto &record : records)
            response->setAuthoritativeAnswer(i++, record);
    }
}

void DnsServiceSimplified::handleNotImplemented(const inet::Packet *packet, inet::Ptr<const DnsRequest> request)
{
    // Select the not implemented response
    auto response = makeShared<DnsRequest>(*request);
    response->setReturnCode(ReturnCode::NOTIMP);
    sendResponseTo(packet, response);
}

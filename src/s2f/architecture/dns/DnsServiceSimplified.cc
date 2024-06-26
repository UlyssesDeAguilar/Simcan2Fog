#include "../../net/dns/DnsServiceSimplified.h"
using namespace dns;
Define_Module(DnsServiceSimplified);

void DnsServiceSimplified::initialize(int stage)
{
    // This layer could be initialized earlier, maybe in INITSTAGE_TRANSPORT_LAYER
    switch (stage)
    {
    case INITSTAGE_LOCAL:
    {
        // serverName = par("serverName");
        break;
    }
    case INITSTAGE_APPLICATION_LAYER:
        // localAddress = L3AddressResolver().addressOf(getParentModule());
        scanNetwork();
        break;
    };
    ApplicationBase::initialize(stage);
}

void DnsServiceSimplified::finish()
{
    // Reset the records for the next simulation
    records.clear();
}

void DnsServiceSimplified::scanNetwork()
{
    cTopology topo;
    ResourceRecord r;
    r.type = RR_Type::NS;

    topo.extractByProperty("servicenode");

    if (topo.getNumNodes() == 0)
        error("Couldn't find the nodes in the topology");

    for (int i = 0; i < topo.getNumNodes(); i++)
    {
        cTopology::Node *node = topo.getNode(i);
        cModule *module = node->getModule();
        L3Address address = L3AddressResolver().addressOf(module->getSubmodule("stack"));
        const char *domain = module->par("serviceDeployed");

        r.ip = address;
        r.domain = domain;
        records[domain].push_back(r);
        EV << "Service: \"" << domain << "\" located on: " << module->getName() << " with ip: " << address << "\n";
    }
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
        auto vec = processQuestion(request->getQuestion(i));

        if (vec)
        {
            ok = allocateIfNull(ok, request.get());
            ok->insertQuestion(request->getQuestion(i));

            for (const auto &record : *vec)
            {
                ok->insertAuthoritativeAnswers(record);
            }
        }
        else
        {
            error = allocateIfNull(error, request.get());
            error->insertQuestion(request->getQuestion(i));
        }
    }

    // Indicate everything went OK
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

std::vector<ResourceRecord> *DnsServiceSimplified::processQuestion(const char *domain)
{
    auto iter = records.find(domain);
    if (iter == records.end())
        return nullptr;
    else
        return &iter->second;
}

void DnsServiceSimplified::handleNotImplemented(const Packet *packet)
{
    // Select the not implemented response
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    auto response = request->dup();
    response->setReturnCode(ReturnCode::NOTIMP);
    sendResponseTo(packet, response);
}

void DnsServiceSimplified::printRecords()
{
    EV << "Record table: " << endl;

    // Print all the records (const & avoids copying the elements)
    for (auto const &elem : records)
    {
        EV << elem.first.c_str() << " :\n";
        for (auto const &record : elem.second)
            EV << record << "\n";
    }
}
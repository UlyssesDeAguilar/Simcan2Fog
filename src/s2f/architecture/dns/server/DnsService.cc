#include "../../net/dns/DnsService.h"
using namespace dns;
Define_Module(DnsService);

void DnsService::initialize(int stage)
{
    // This layer could be initialized earlier, maybe in INITSTAGE_TRANSPORT_LAYER
    switch (stage)
    {
    case INITSTAGE_LOCAL:
    {
        mode = parseMode(par("mode"));
        serverName = par("serverName");

        if (mode == NS)
        {
            cStringTokenizer tokenizer(serverName, ".");
            fqdn = tokenizer.asVector();
        }

        WATCH(mode);
        WATCH(serverName);
        // WATCH_MAP(records);
        break;
    }
    case INITSTAGE_APPLICATION_LAYER:
        if (mode == ROOT || mode == TLD)
            scanNetwork();

        localAddress = L3AddressResolver().addressOf(getParentModule());
        break;
    };
    ApplicationBase::initialize(stage);
}

DnsService::Mode DnsService::parseMode(const char *mode)
{
    if (*mode)
    {
        if (strncmp(mode, "root", 5) == 0)
            return ROOT;
        if (strncmp(mode, "tld", 4) == 0)
            return TLD;
        if (strncmp(mode, "ns", 3) == 0)
            return NS;
        error("Unkown dns server mode");
    }
    else
        error("Empty server mode");
    return NS;
}

void DnsService::scanNetwork()
{
    cTopology topo;
    ResourceRecord r;
    r.type = RR_Type::NS;

    if (mode == ROOT)
    {
        std::vector<std::string> nedTypes;
        nedTypes.push_back("DnsTldServer");
        topo.extractByNedTypeName(nedTypes);
    }
    else if (mode == TLD)
    {
        topo.extractByProperty("dnsTld", serverName);
    }

    if (topo.getNumNodes() == 0)
        error("Couldn't find the nodes in the topology, mode: %d", mode);

    for (int i = 0; i < topo.getNumNodes(); i++)
    {
        cTopology::Node *node = topo.getNode(i);
        cModule *module = node->getModule();
        L3Address address = L3AddressResolver().addressOf(module);
        const char *domain = module->getSubmodule("dnsServer")->par("serviceName");

        r.ip = address;
        r.domain = domain;
        records[domain].push_back(r);
    }
}

void DnsService::finish()
{
    // Reset the records for the next simulation
    records.clear();
}

void DnsService::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(DNS_PORT);
}

void DnsService::socketDataArrived(UdpSocket *socket, Packet *packet)
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

void DnsService::sendResponseTo(const Packet *packet, DnsRequest *response)
{
    // Retrieve IP:Port from sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int remotePort = packet->getTag<L4PortInd>()->getSrcPort();

    // Process and package response
    auto responsePacket = new Packet("DnsRequest", Ptr<DnsRequest>(response));
    socket.sendTo(responsePacket, remoteAddress, remotePort);
}

void DnsService::socketErrorArrived(UdpSocket *socket, Indication *indication) {}

void DnsService::handleQuery(const Packet *packet)
{
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
        DnsRequest *p;
        auto vec = processQuestion(request->getQuestion(i));

        if (vec)
        {
            p = allocateIfNull(ok, request.get());
            p->insertQuestion(request->getQuestion(i));
            for (const auto &record : *vec)
            {
                if (mode == NS)
                    p->insertAuthoritativeAnswers(record);
                else
                    p->insertNonAuthoritativeAnswers(record);
            }
        }
        else
        {
            p = allocateIfNull(error, request.get());
            p->insertQuestion(request->getQuestion(i));
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

DnsRequest *DnsService::prepareResponse(const DnsRequest *request)
{
    auto response = new DnsRequest();
    response->setOperationCode(request->getOperationCode());
    response->setRequestId(request->getRequestId());
    response->setIsAuthoritative(mode == NS);
    return response;
}

std::vector<ResourceRecord> *DnsService::processQuestion(const char *domain)
{
    cStringTokenizer tokenizer(domain, ".");
    const auto tokens = tokenizer.asVector();
    const char *entry;

    // Sanity check
    if (tokens.size() < 3)
        error("Less than three tokens while parsing domain name, it's malformed");

    switch (mode)
    {
    case ROOT:
        entry = tokens.back().c_str();
        break;
    case TLD:
        entry = tokens[tokens.size() - 1].c_str();
        break;
    case NS:
        entry = tokens[tokens.size() - 2].c_str();
        break;
    }

    auto iter = records.find(entry);
    if (iter == records.end())
        return nullptr;
    else
        return &iter->second;
}

void DnsService::registerService(const char *serviceName)
{
    // service.dc.com
    std::ostringstream stream;
    stream << serviceName << "." << fqdn[2] << "." << fqdn[3];

    ResourceRecord r;
    r.type = RR_Type::A;
    r.domain = stream.str().c_str();
    r.ip = localAddress;

    records[serviceName].push_back(r);
}

void DnsService::unregisterService(const char *serviceName)
{
    records.erase(records.find(serviceName));
}

void DnsService::handleNotImplemented(const Packet *packet)
{
    // Select the not implemented response
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    auto response = request->dup();
    response->setReturnCode(ReturnCode::NOTIMP);
    sendResponseTo(packet, response);
}

void DnsService::printRecords()
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
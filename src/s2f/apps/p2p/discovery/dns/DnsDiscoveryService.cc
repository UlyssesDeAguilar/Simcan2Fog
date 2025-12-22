#include "DnsDiscoveryService.h"
#include "inet/common/InitStages.h"
#include "inet/networklayer/common/L3Address.h"
#include "s2f/apps/AppBase.h"
#include "s2f/apps/p2p/InnerRequest_m.h"
#include "s2f/apps/p2p/discovery/DiscoveryResolution_m.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"

Define_Module(DnsDiscoveryService);

using namespace s2f::dns;

void DnsDiscoveryService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        AppBase::initialize();
        setReturnCallback(this);
        dnsSeed = par("dnsSeed");

        dnsSock = open(-1, SOCK_STREAM);
        dnsIp = L3Address(par("dnsIp"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        localIp = L3AddressResolver().addressOf(getModuleByPath("^.^."));
        connect(dnsSock, dnsIp, 443);
    }
}

void DnsDiscoveryService::handleMessage(omnetpp::cMessage *msg)
{

    auto arrivalGate = msg->getArrivalGate();
    if (!arrivalGate || strncmp(arrivalGate->getName(), "internal", 9) != 0)
    {
        AppBase::handleMessage(msg);
        return;
    }

    // Handle a discovery request
    auto request = static_cast<InnerRequest *>(msg);

    caller = getSimulation()->getModule(request->getModuleId());
    resolve(dnsSeed);

    delete msg;
}

void DnsDiscoveryService::handleResolutionFinished(const std::set<L3Address> ipResolutions, bool resolved)
{
    if (!resolved)
        error("No peers connected on DNS seed %s.", dnsSeed);

    EV << "DNS seed resolved to " << ipResolutions.size()
       << " possible addresses" << "\n";

    // Add peer candidates from resolution

    const auto &response = new DiscoveryResolution("DNS");

    for (const auto &ip : ipResolutions)
        response->appendResolution(ip);

    sendDirect(response, caller, "discovery");
}

void DnsDiscoveryService::handleConnectReturn(int sockFd, bool connected)
{
    if (sockFd != dnsSock)
        return;
    if (!connected)
        error("Cannot connect to DNS to join the network");

    EV << "Registering node with ip" << localIp
       << " on dns seed " << dnsSeed << "\n";

    ResourceRecord record;

    auto packet = new Packet("Registration request");
    const auto &request = makeShared<DnsRegistrationRequest>();

    record.type = RecordType::A;
    record.domain = dnsSeed;
    record.ip = localIp;

    request->appendRecords(record);
    request->setZone("mainnet.com"); // TODO: parse from dnsSeed
    request->setVerb(Verb::PUT);

    packet->insertAtBack(request);
    _send(dnsSock, packet);
}

void DnsDiscoveryService::handleDataArrived(int sockFd, Packet *p)
{
    auto httpResponse = p->peekData<RestfulResponse>();
    if (httpResponse->getResponseCode() == ResponseCode::OK)
        EV_INFO << "Service registered at DNS seed " << dnsSeed << "\n";

    delete p;
}

void DnsDiscoveryService::finish()
{
    close(dnsSock);
    AppBase::finish();
}

#include "P2PBase.h"
#include "inet/common/Ptr.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"
#include "s2f/messages/Syscall_m.h"
#include <algorithm>

using namespace s2f::dns;

Define_Module(P2PBase);

// ------------------------------------------------------------------------- //
//                                  OVERRIDES                                //
// ------------------------------------------------------------------------- //
void P2PBase::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);

    // DNS seed resolution params
    dnsSock = open(-1, SOCK_STREAM);
    dnsSeed = par("dnsSeed");
    dnsIp = L3Address(par("dnsIp"));

    // P2P params
    peers.clear();
    resolutionList.clear();

    listeningPort = par("listeningPort");
    discoveryAttempts = par("discoveryAttempts");
    discoveryThreshold = par("discoveryThreshold");

    // Simulation params
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void P2PBase::processSelfMessage(cMessage *msg)
{

    if (msg->getKind() == EXEC_START)
    {
        // Open the P2P protocol socket
        localIp = L3AddressResolver().addressOf(getModuleByPath("^.^."));
        listeningSocket = open(listeningPort, SOCK_STREAM);
        listen(listeningSocket);

        // Join the network at an arbitrary time
        event->setKind(NODE_UP);
        scheduleAfter(uniform(30, 120), event);
    }
    else if (msg->getKind() == PEER_CONNECTION)
    {
        if (!resolutionList.empty())
        {
            connectToPeer();
            scheduleAfter(1.0, event);
        }
        else
        {
            discoveryAttempts--;
            event->setKind(PEER_DISCOVERY);
            scheduleAfter(1.0, event);
        }
    }
}

// TODO: remove from base class
void P2PBase::handleResolutionFinished(const std::set<L3Address> ipResolutions,
                                       bool resolved)
{
    if (!resolved)
        error("No peers connected on DNS seed %s.", dnsSeed);

    EV_INFO << "DNS seed resolved to " << ipResolutions.size()
            << " possible addresses" << "\n";

    // Add peer candidates from resolution
    for (const auto &ip : ipResolutions)
        if (ip != localIp && !peers.count(ip))
            resolutionList.push_back(ip);

    // Start connecting to discovered peers
    event->setKind(PEER_CONNECTION);
    scheduleAfter(1.0, event);
}

// TODO: remove from base class
void P2PBase::handleConnectReturn(int sockFd, bool connected)
{
    if (!connected)
        error("Cannot connect to DNS to join the network");

    EV_INFO << "Registering service on port " << listeningPort
            << " on dns seed " << dnsSeed << "\n";

    // Register as peer into DNS seed
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

// TODO: remove from base class
void P2PBase::handleDataArrived(int sockFd, Packet *p)
{
    auto httpResponse = p->peekData<RestfulResponse>();
    if (httpResponse->getResponseCode() == ResponseCode::OK)
        EV_INFO << "Service registered at DNS seed " << dnsSeed << "\n";

    event->setKind(PEER_DISCOVERY);
    scheduleAfter(1.0, event);
}

bool P2PBase::handlePeerClosed(int sockFd)
{
    EV << "Peer" << peerData[sockFd]->getIpAddress() << "closed the connection"
       << "\n";
    peers.erase(peerData[sockFd]->getIpAddress());
    delete peerData[sockFd];
    peerData.erase(sockFd);
    return true;
}

void P2PBase::finish()
{
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << "Testing P2P Application shutdown." << '\n';
    EV_INFO << " + Total simulation time (simulated):"
            << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds"
            << '\n';

    for (auto &[sockFd, p] : peerData)
    {
        close(sockFd);
        delete p;
    }

    peers.clear();
    resolutionList.clear();
    close(dnsSock);

    // Finish the super-class
    AppBase::finish();
}

// ------------------------------------------------------------------------- //
//                                 METHODS                                   //
// ------------------------------------------------------------------------- //

void P2PBase::handleConnectFailure(int sockFd)
{
    EV_INFO << "Connection failed for IP " << peerData[sockFd]->getIpAddress()
            << "on sockFd" << sockFd << "\n";
    peers.erase(peerData[sockFd]->getIpAddress());
    delete peerData[sockFd];
    peerData.erase(sockFd);
}

void P2PBase::connectToPeer()
{
    L3Address destIp = resolutionList.back();
    resolutionList.pop_back();

    if (peers.count(destIp))
        return;

    int sockFd = open(-1, SOCK_STREAM);
    connect(sockFd, destIp, listeningPort);
}

int P2PBase::getSockFd(L3Address ip)
{
    auto it = std::find_if(peerData.begin(), peerData.end(), [&](const auto &p)
                           { return p.second->getIpAddress() == ip; });
    return it != peerData.end() ? it->first : 0;
}

#include "P2PBase.h"
#include "inet/common/Ptr.h"
#include "inet/common/Simsignals_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"
#include "s2f/messages/Syscall_m.h"

using namespace s2f::dns;

Define_Module(P2PBase);

void P2PBase::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);
    connectionQueue.clear();

    listeningPort = par("listeningPort");
    dnsSeed = par("dnsSeed");
    dnsIp = L3Address(par("dnsIp"));
    localIp = L3Address(par("localIp"));

    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void P2PBase::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == EXEC_START)
    {
        // Open the designated port to handle P2P requests
        listeningSocket = open(listeningPort, SOCK_STREAM);
        listen(listeningSocket);

        // TODO: register service in the dns seed
        EV_INFO << "Registering service on port " << listeningPort
                << " on dns seed " << dnsSeed << "\n";
    }

    // Temporary: Reach peer through IP
    NetworkPeer peer = {.sockFd = open(-1, SOCK_STREAM),
                        .ip = L3Address(par("testIp"))};
    peerCandidates.push_back(peer);
    connect(peer.sockFd, peer.ip, listeningPort);
}

void P2PBase::handleConnectFailure()
{
    NetworkPeer peer = peerCandidates.back();
    EV_INFO << "Could not reach peer candidate" << peer.ip << "\n";

    // Remove candidate on reconnection threshold break
    if (!peer.reconnections--)
        peerCandidates.pop_back();

    // Out of candidates to process
    if (peerCandidates.empty())
    {
        event->setKind(NO_ROUTE_FOUND); // TODO: connection status enum
        scheduleAt(simTime() + 1.0, event);
    }

    // Attempt a new connection
    peer = peerCandidates.back();
    connect(peer.sockFd, peer.ip, listeningPort);
}

void P2PBase::handleResolutionFinished(const L3Address ip, bool resolved)
{
    if (!resolved)
        error("No peers connected on DNS seed %s.", dnsSeed);
    else if (ip == localIp)
    {
        EV_INFO << "DNS seed resolved to our address, discarding..." << "\n";
        return;
    }

    // Attempt connection to peer candidate
    NetworkPeer peer = {.sockFd = open(-1, SOCK_STREAM), .ip = ip};
    peerCandidates.push_back(peer);
    connect(peer.sockFd, peer.ip, listeningPort);
}

bool P2PBase::handleClientConnection(int sockFd, const L3Address &remoteIp,
                                     const uint16_t &remotePort)
{
    EV << "Client connected: " << remoteIp << ":" << remotePort << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    // Create the chunk queue
    connectionQueue[sockFd];
    return true;
}

bool P2PBase::handlePeerClosed(int sockFd)
{
    EV << "Peer closed, session ended\n";
    connectionQueue.erase(sockFd);
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

    // Finish the super-class
    AppBase::finish();
}

void P2PBase::registerServiceToDNS()
{
    int sockFd = open(-1, SOCK_DGRAM);
    ResourceRecord record;

    auto packet = new Packet("Registration request");
    const auto &request = makeShared<DnsRegistrationRequest>();

    record.type = RecordType::A;
    record.domain = dnsSeed;
    record.ip = localIp;

    request->setRecordsArraySize(1);
    request->setRecords(0, record);
    request->setZone("mainnet.com");
    packet->insertAtBack(request);

    _send(sockFd, packet, dnsIp.toIpv4().getInt(), 53);
}

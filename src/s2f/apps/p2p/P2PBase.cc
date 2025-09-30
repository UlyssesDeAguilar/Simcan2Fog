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
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"
#include "s2f/architecture/p2p/pow/PowMsgAddress_m.h"
#include "s2f/messages/Syscall_m.h"

using namespace s2f::dns;

Define_Module(P2PBase);

// ------------------------------------------------------------------------- //
//                             P2PBASE METHODS                               //
// ------------------------------------------------------------------------- //
void P2PBase::handleConnectFailure(int sockFd)
{
    EV_INFO << "Could not reach peer " << peers[sockFd]->ipAddress << "\n";

    // Remove peer from active connection list
    peers.erase(sockFd);

    // Out of candidates to process
    if (peerCandidates.empty())
    {
        event->setKind(NO_ROUTE_FOUND); // TODO: connection status enum
        scheduleAt(simTime() + 1.0, event);
        return;
    }

    // Attempt a new connection
    connectToPeer();
}

void P2PBase::connectToPeer(const L3Address &destIp)
{
    int sockFd = open(-1, SOCK_STREAM);
    peers[sockFd] = new NetworkPeer;
    peers[sockFd]->ipAddress = destIp;
    connect(sockFd, peers[sockFd]->ipAddress, listeningPort);
}

void P2PBase::connectToPeer()
{
    int sockFd = open(-1, SOCK_STREAM);
    peers[sockFd] = peerCandidates.back();
    peerCandidates.pop_back();
    connect(sockFd, peers[sockFd]->ipAddress, listeningPort);
}

// ------------------------------------------------------------------------- //
//                            APPBASE OVERRIDES                              //
// ------------------------------------------------------------------------- //

void P2PBase::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);
    connectionQueue.clear();

    dnsSock = open(-1, SOCK_STREAM);
    listeningPort = par("listeningPort");
    dnsSeed = par("dnsSeed");
    dnsIp = L3Address(par("dnsIp"));
    localIp = L3Address(par("localIp"));

    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void P2PBase::processSelfMessage(cMessage *msg)
{

    // Open the designated port to handle P2P requests
    if (msg->getKind() == EXEC_START)
    {
        listeningSocket = open(listeningPort, SOCK_STREAM);
        listen(listeningSocket);

        EV_INFO << "Connecting to DNS registry service" << "\n";
        connect(dnsSock, dnsIp, 443);
    }
    return;

    // Temporary: Reach peer through IP
    if (strcmp(par("localIp"), "10.0.0.1") == 0)
        connectToPeer(L3Address(par("testIp")));
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
    connectToPeer(ip);
}

void P2PBase::handleConnectReturn(int sockFd, bool connected)
{
    if (connected == false)
        error("Cannot connect to DNS to join the network");

    EV_INFO << "Registering service on port " << listeningPort
            << " on dns seed " << dnsSeed << "\n";
    ResourceRecord record;

    auto packet = new Packet("Registration request");
    const auto &request = makeShared<DnsRegistrationRequest>();

    record.type = RecordType::A;
    record.domain = dnsSeed;
    record.ip = localIp;

    request->appendRecords(record);
    request->setZone("mainnet.com");
    request->setVerb(Verb::PUT);

    packet->insertAtBack(request);
    _send(dnsSock, packet);
}

void P2PBase::handleDataArrived(int sockFd, Packet *p)
{

    auto response = p->peekData<RestfulResponse>();
    EV_INFO << "Received response code " << response->getResponseCode() << " from DNS service" << "\n";
}

bool P2PBase::handlePeerClosed(int sockFd)
{
    EV << "Peer" << peers[sockFd]->ipAddress << "closed the connection" << "\n";
    connectionQueue.erase(sockFd);
    delete peers[sockFd];
    peers.erase(sockFd);
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

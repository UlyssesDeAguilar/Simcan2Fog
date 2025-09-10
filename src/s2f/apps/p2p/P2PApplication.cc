#include "P2PApplication.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "s2f/apps/AppBase.h"
#include "s2f/messages/Syscall_m.h"

Define_Module(P2PApplication);

void P2PApplication::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);

    listeningPort = MAIN_NET;
    state = DISCOVERY_DNSSEED;
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void P2PApplication::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == EXEC_START)
    {
        // Open the designated port for p2p petition handling
        listeningSocket =
            open(listeningPort, SOCK_STREAM, par("localInterface"));
        registerService(dnsSeed, listeningSocket);

        // Peer discovery through DNS records
        resolve(dnsSeed);
    }
    else
    {
    }
}

void P2PApplication::handleResolutionFinished(const L3Address ip, bool resolved)
{
    if (resolved)
    {
        NetworkPeer peer =
            NetworkPeer(ip, open(-1, SOCK_STREAM, par("externalInterface")));
        peerCandidates.push_back(peer);
        connect(peer.sockFd, peer.ip, listeningPort);
    }
}

void P2PApplication::handleConnectReturn(int sockFd, bool connected)
{
    if (connected)
    {
        // TODO: send 'version' message
    }

    peerCandidates.pop_back();

    if (!peerCandidates.empty())
    {
        NetworkPeer peer = peerCandidates.back();
        connect(peer.sockFd, peer.ip, listeningPort);
    }
    else if (peers.empty())
    {
        // TODO: send self-message to change connection status
        scheduleAt(simTime() + 1.0, event);
    }
}

void P2PApplication::handleDataArrived(int sockFd, Packet *p)
{
    // TODO: handle message type
}

bool P2PApplication::handlePeerClosed(int sockFd) { return true; }
void P2PApplication::finish()
{
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << " + Total simulation time (simulated):"
            << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds"
            << '\n';

    // Finish the super-class
    AppBase::finish();
}

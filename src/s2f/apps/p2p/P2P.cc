#include "P2P.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "s2f/apps/AppBase.h"
#include "s2f/messages/Syscall_m.h"
#include <algorithm>

Define_Module(P2P);

// ------------------------------------------------------------------------- //
//                                  OVERRIDES                                //
// ------------------------------------------------------------------------- //
void P2P::initialize()
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

void P2P::processSelfMessage(cMessage *msg)
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

bool P2P::handlePeerClosed(int sockFd)
{
    EV << "Peer" << peerData[sockFd]->getIpAddress() << "closed the connection"
       << "\n";
    peers.erase(peerData[sockFd]->getIpAddress());
    delete peerData[sockFd];
    peerData.erase(sockFd);
    return true;
}

void P2P::finish()
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

void P2P::handleConnectFailure(int sockFd)
{
    EV_INFO << "Connection failed for IP " << peerData[sockFd]->getIpAddress()
            << "on sockFd" << sockFd << "\n";
    peers.erase(peerData[sockFd]->getIpAddress());
    delete peerData[sockFd];
    peerData.erase(sockFd);
}

void P2P::connectToPeer()
{
    L3Address destIp = resolutionList.back();
    resolutionList.pop_back();

    if (peers.count(destIp))
        return;

    int sockFd = open(-1, SOCK_STREAM);
    connect(sockFd, destIp, listeningPort);
}

int P2P::getSockFd(L3Address ip)
{
    auto it = std::find_if(peerData.begin(), peerData.end(), [&](const auto &p)
                           { return p.second->getIpAddress() == ip; });
    return it != peerData.end() ? it->first : 0;
}

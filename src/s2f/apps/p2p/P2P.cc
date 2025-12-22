#include "P2P.h"
#include "discovery/DiscoveryResolution_m.h"
#include "inet/common/InitStages.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "s2f/apps/AppBase.h"
#include "s2f/apps/p2p/InnerRequest_m.h"
#include "s2f/messages/Syscall_m.h"

Define_Module(P2P);

// ------------------------------------------------------------------------- //
//                                  OVERRIDES                                //
// ------------------------------------------------------------------------- //
void P2P::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        AppBase::initialize();
        setReturnCallback(this);

        // P2P params
        peers.clear();
        resolutionList.clear();

        listeningPort = par("listeningPort");
        discoveryAttempts = par("discoveryAttempts");
        discoveryThreshold = par("discoveryThreshold");

        // Simulation params
        simStartTime = simTime();
        runStartTime = time(nullptr);

        discoveryService = getModuleByPath(par("discoveryPath"))->gate("internal");
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        localIp = L3AddressResolver().addressOf(getModuleByPath("^.^."));
        listeningSocket = open(listeningPort, SOCK_STREAM);
        listen(listeningSocket);
    }
}

void P2P::handleMessage(cMessage *msg)
{
    auto arrivalGate = msg->getArrivalGate();
    EV << "soy de discovery...?\n";

    if (arrivalGate && strncmp(arrivalGate->getName(), "discovery", 9) == 0)
    {
        EV << "es aca...?\n";
        auto response = check_and_cast<DiscoveryResolution *>(msg);

        EV << "o es aca...?\n";
        for (int i = 0; i < response->getResolutionArraySize(); i++)
            resolutionList.insert(response->getResolution(i));

        EV << "aca ni de chiste...?\n";
        delete msg;
    }
    else
        AppBase::handleMessage(msg);

    EV << "si sali, tenemos problemS...?\n";
}

void P2P::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == EXEC_START)
    {
        // Join the network at an arbitrary time
        event->setKind(PEER_DISCOVERY);
        scheduleAfter(uniform(30, 120), event);
    }
    else if (msg->getKind() == PEER_DISCOVERY)
    {
        if (peers.size() < discoveryThreshold && discoveryAttempts > 0)
        {
            auto discoveryRequest = new InnerRequest("discovery");
            discoveryRequest->setModuleId(getId());
            sendDirect(discoveryRequest, discoveryService);
        }
        else if (peers.size() > 0)
            event->setKind(CONNECTED);
        else
        {
            // NOTE: temporary fallback to avoid infinite loops
        }
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
    L3Address destIp = *resolutionList.begin();
    resolutionList.erase(resolutionList.begin());

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

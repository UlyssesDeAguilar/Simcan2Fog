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
        discoveryServices.clear();

        listeningPort = par("listeningPort");
        discoveryAttempts = par("discoveryAttempts");
        discoveryThreshold = par("discoveryThreshold");

        // Discovery Services
        std::string discoveryPath = par("discoveryPath");
        numServices = getParentModule()->getParentModule()->par("numServices");
        discoveryServices.reserve(numServices);

        for (int i = 0; i < numServices; i++)
        {
            std::string path = discoveryPath + "[" + std::to_string(i) + "].app";
            discoveryServices.push_back(getModuleByPath(path.c_str())->gate("internal"));
        }

        // Simulation params
        simStartTime = simTime();
        runStartTime = time(nullptr);
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

    // Non-discovery messages handled by the parent
    if (!arrivalGate || strncmp(arrivalGate->getName(), "discovery", 9) != 0)
    {
        AppBase::handleMessage(msg);
        return;
    }

    // Add all IP addressess as candidates
    auto response = check_and_cast<DiscoveryResolution *>(msg);

    for (int i = 0; i < response->getResolutionArraySize(); i++)
        resolutionList.insert(response->getResolution(i));

    delete msg;

    // Handle state transition
    numServices--;

    if (numServices)
        return;

    event->setKind(PEER_CONNECTION);
    scheduleAfter(1.0, event);
    numServices = discoveryServices.size();
}

void P2P::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == EXEC_START)
    {
        // Join the network at an arbitrary time
        event->setKind(PEER_DISCOVERY);
        scheduleAfter(uniform(30, 120), event);
    }
    else if (msg->getKind() == PEER_DISCOVERY && discoveryAttempts)
        for (auto &service : discoveryServices)
        {
            auto discoveryRequest = new InnerRequest("discovery");
            discoveryRequest->setModuleId(getId());
            sendDirect(discoveryRequest, service);
        }
    else if (msg->getKind() == PEER_CONNECTION)
    {
        if (!resolutionList.empty())
        {
            connectToPeer();
            scheduleAfter(1.0, event);
        }
        else if (peers.size() < discoveryThreshold)
        {
            discoveryAttempts--;
            event->setKind(PEER_DISCOVERY);
            scheduleAfter(uniform(30, 120), event);
        }
        else
        {
            event->setKind(CONNECTED);
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

    if (peers.count(destIp) || destIp == localIp)
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

#include "PowP2PApp.h"
#include "handlers/AddressMessageHandler.h"
#include "handlers/GetAddressMessageHandler.h"
#include "handlers/VerackMessageHandler.h"
#include "handlers/VersionMessageHandler.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/handlers/IMessageHandler.h"
#include "s2f/architecture/p2p/pow/handlers/InitialMessageHandler.h"
#include <memory>

using namespace s2f::p2p;
Define_Module(PowP2PApp);

// ------------------------------------------------------------------------- //
//                             P2PBASE OVERRIDES                             //
// ------------------------------------------------------------------------- //

void PowP2PApp::initialize()
{
    peerConnection.clear();
    handlers.emplace("initial", std::make_unique<InitialMessageHandler>());
    handlers.emplace("version", std::make_unique<VersionMessageHandler>());
    handlers.emplace("verack", std::make_unique<VerackMessageHandler>());
    handlers.emplace("getaddr", std::make_unique<GetAddressMessageHandler>());
    handlers.emplace("addr", std::make_unique<AddressMessageHandler>());

    self.setServices(pow::NODE_NETWORK);
    self.setTime(time(nullptr));
    P2PBase::initialize();
}

void PowP2PApp::finish()
{
    for (auto &[_, event] : peerConnection)
        cancelAndDelete(event);

    peerConnection.clear();
    handlers.clear();
    P2PBase::finish();
}

void PowP2PApp::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == NODE_UP)
    {
        self.setIpAddress(localIp);
        // TODO: NODE_UP of non-full nodes, which do not connect to the dns
        EV << "Connecting to DNS registry service" << "\n";
        connect(dnsSock, dnsIp, 443);
    }
    else if (msg->getKind() == PEER_DISCOVERY)
    {
        // TODO: define more peer discovery methods
        if (peers.size() < discoveryThreshold && discoveryAttempts > 0)
            resolve(dnsSeed);
        else if (peers.size() > 0)
            event->setKind(CONNECTED);
        else
        {
            // NOTE: temporary fallback to avoid infinite loops
        }
    }
    else
        P2PBase::processSelfMessage(msg);
}

void PowP2PApp::handleConnectReturn(int sockFd, bool connected)
{
    if (sockFd == dnsSock)
    {
        P2PBase::handleConnectReturn(sockFd, connected);
        return;
    }

    if (!connected)
    {
        handleConnectFailure(sockFd);
        return;
    }

    PowNetworkPeer *p;
    TcpSocket *sock;
    int oldSock;
    HandlerContext ictx = {powPeers, isClient(sockFd), sockFd, self, localIp};

    EV << "Handling connect return on peer with ip " << localIp << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    // TODO: check why close(sockFd) if both peers close it at the same time.
    sock = check_and_cast<TcpSocket *>(socketMap.getSocketById(sockFd));
    oldSock = findIpInPeers(sock->getRemoteAddress());

    if (oldSock)
    {
        p = powPeers[oldSock];

        // Remove duplicate connections
        if (p->getState() == ConnectionState::CONNECTED)
        {
            EV << "Found active connection to same peer on socket fd: " << oldSock << ". Closing.\n";
            return;
        }

        // Remove stale candidate on differing sockets
        // Reinserts at same index if oldSock == sockFd (peers[sockFd] = p)
        powPeers.erase(oldSock);
    }
    else
    {
        p = new PowNetworkPeer;
        p->setServices(pow::UNNAMED);
    }

    p->setIpAddress(sock->getRemoteAddress());
    p->setPort(sock->getRemotePort());
    p->setState(ConnectionState::CONNECTED);

    peers[sockFd] = p;
    peerConnection[sockFd] = new cMessage("CONNECTION STATUS");

    self.setPort(sock->getLocalPort());

    auto response = handlers["initial"]->buildResponse(ictx);

    if (response)
        _send(sockFd, response);
}

void PowP2PApp::handleDataArrived(int sockFd, Packet *p)
{

    if (sockFd == dnsSock)
    {
        P2PBase::handleDataArrived(sockFd, p);
        return;
    }

    EV << "Packet arrived from peer with connection " << sockFd << "\n";

    auto header = p->popAtFront<Header>();
    auto handler = handlers.find(header->getCommandName());
    HandlerContext ictx = {powPeers, isClient(sockFd), sockFd, self, localIp};

    if (handler == handlers.end())
        error("Message kind not yet implemented! %s", header->getCommandName());

    auto result = handler->second->handleMessage(p, ictx);
    auto response = handler->second->buildResponse(ictx);

    switch (result.action)
    {
    case OPEN:
        for (auto &p : result.peers)
        {
            p->setPort(listeningPort);
            powPeers[open(-1, SOCK_STREAM)] = p;
        }
        break;
    case SCHEDULE:
        scheduleAfter(20 * 60, peerConnection[sockFd]);
        break;
    case CANCEL:
        cancelEvent(peerConnection[sockFd]);
        break;
    default:
        break;
    }

    if (response)
        _send(sockFd, response);
}

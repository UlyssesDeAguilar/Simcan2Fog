#include "PowP2PApp.h"
#include "handlers/AddressMessageHandler.h"
#include "handlers/GetAddressMessageHandler.h"
#include "handlers/VerackMessageHandler.h"
#include "handlers/VersionMessageHandler.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/handlers/IMessageHandler.h"
#include "s2f/architecture/p2p/pow/handlers/InitialMessageBuilder.h"
#include "s2f/architecture/p2p/pow/handlers/PingMessageHandler.h"
#include "s2f/architecture/p2p/pow/handlers/PongMessageHandler.h"
#include <memory>

using namespace s2f::p2p;
Define_Module(PowP2PApp);

// ------------------------------------------------------------------------- //
//                             P2PBASE OVERRIDES                             //
// ------------------------------------------------------------------------- //

void PowP2PApp::initialize()
{
    peerConnection.clear();
    initialHandler = std::make_unique<InitialMessageBuilder>();
    handlers.emplace("version", std::make_unique<VersionMessageHandler>());
    handlers.emplace("verack", std::make_unique<VerackMessageHandler>());
    handlers.emplace("getaddr", std::make_unique<GetAddressMessageHandler>());
    handlers.emplace("addr", std::make_unique<AddressMessageHandler>());
    handlers.emplace("ping", std::make_unique<PingMessageHandler>());
    handlers.emplace("pong", std::make_unique<PongMessageHandler>());

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

void PowP2PApp::processConnectionState(cMessage *msg)
{
    if (msg->getKind() == pow::SEND_PING)
    {
        HandlerContext ictx = {.peers = powPeers, .isClient = true, .self = self};
        int sockFd = *(int *)msg->getContextPointer();

        if (powPeers.count(sockFd))
        {
            peerConnection[sockFd]->setKind(pow::AWAITING_POLL);
            scheduleAfter(pow::PONG_TIMEOUT_SECONDS, peerConnection[sockFd]);
            _send(sockFd, initialHandler->buildMessage("ping", ictx));
        }
        else
        {
            delete peerConnection[sockFd];
            peerConnection.erase(sockFd);
        }
        delete (int *)msg->getContextPointer();
    }
    else if (msg->getKind() == pow::AWAITING_POLL)
    {
        // TODO: add disconnection after timeout
    }
}

void PowP2PApp::processNodeState(cMessage *msg)
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
    HandlerContext ictx = {
        .peers = powPeers,
        .isClient = isClient(sockFd),
        .sockFd = sockFd,
        .self = self,
        .localIp = localIp,
    };

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

    auto response = initialHandler->buildMessage("version", ictx);

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
    HandlerContext ictx = {
        .msg = p,
        .peers = powPeers,
        .isClient = isClient(sockFd),
        .sockFd = sockFd,
        .self = self,
        .localIp = localIp,
    };

    if (handler == handlers.end())
        error("Message kind not yet implemented! %s", header->getCommandName());

    auto result = handler->second->handleMessage(ictx);
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
    case CANCEL:
        cancelEvent(peerConnection[sockFd]);
    case SCHEDULE:
        if (result.eventDelayMin)
        {
            peerConnection[sockFd]->setKind(result.eventKind);
            peerConnection[sockFd]->setContextPointer(static_cast<void *>(new int(sockFd)));
            scheduleAfter(uniform(result.eventDelayMin, result.eventDelayMax), peerConnection[sockFd]);
        }
        break;
    default:
        break;
    }

    if (response)
        _send(sockFd, response);
}

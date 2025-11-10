#include "PowP2PApp.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowNetworkPeer_m.h"
#include "s2f/architecture/p2p/pow/consumers/AddressMsgConsumer.h"
#include "s2f/architecture/p2p/pow/consumers/GetAddressMsgConsumer.h"
#include "s2f/architecture/p2p/pow/consumers/PingMsgConsumer.h"
#include "s2f/architecture/p2p/pow/consumers/PongMsgConsumer.h"
#include "s2f/architecture/p2p/pow/consumers/VerackMsgConsumer.h"
#include "s2f/architecture/p2p/pow/consumers/VersionMsgConsumer.h"
#include "s2f/architecture/p2p/pow/producers/PingMsgProducer.h"
#include "s2f/architecture/p2p/pow/producers/VersionMsgProducer.h"

#include <memory>

using namespace s2f::p2p::pow;
Define_Module(PowP2PApp);

// ------------------------------------------------------------------------- //
//                                  OVERRIDES                                //
// ------------------------------------------------------------------------- //

void PowP2PApp::initialize()
{
    peerConnection.clear();

    // Message consumers
    consumers.emplace("version", std::make_unique<VersionMsgConsumer>());
    consumers.emplace("verack", std::make_unique<VerackMsgConsumer>());
    consumers.emplace("getaddr", std::make_unique<GetAddressMsgConsumer>());
    consumers.emplace("addr", std::make_unique<AddressMsgConsumer>());
    consumers.emplace("ping", std::make_unique<PingMsgConsumer>());
    consumers.emplace("pong", std::make_unique<PongMsgConsumer>());

    // Message producers
    producers.emplace("version", std::make_unique<VersionMsgProducer>());
    producers.emplace("ping", std::make_unique<PingMsgProducer>());

    self.setServices(pow::NODE_NETWORK);
    self.setTime(time(nullptr));
    P2PBase::initialize();
}

void PowP2PApp::finish()
{
    for (auto &[_, event] : peerConnection)
        cancelAndDelete(event);

    peerConnection.clear();
    consumers.clear();
    producers.clear();
    P2PBase::finish();
}

void PowP2PApp::processConnectionState(cMessage *msg)
{
    if (msg->getKind() == pow::SEND_PING)
    {
        IPowMsgContext ictx = {.peers = powPeers, .isClient = true, .self = self};
        int sockFd = *(int *)msg->getContextPointer();

        if (powPeers.count(sockFd))
        {
            peerConnection[sockFd]->setKind(pow::AWAITING_POLL);
            scheduleAfter(pow::PONG_TIMEOUT_SECONDS, peerConnection[sockFd]);
            _send(sockFd, producers["ping"]->buildMessage(ictx));
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

    // TODO: merge NODE_UP and PEER_DISCOVERY, change for call to various
    // discovery services.
    if (msg->getKind() == NODE_UP)
    {
        self.setIpAddress(localIp);
        EV << "Connecting to DNS registry service" << "\n";
        connect(dnsSock, dnsIp, 443);
    }
    else if (msg->getKind() == PEER_DISCOVERY)
    {
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
    IPowMsgContext ictx = {
        .peers = powPeers,
        .isClient = isClient(sockFd),
        .sockFd = sockFd,
        .self = self,
    };

    EV << "Handling connect return on peer with ip " << localIp << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    // TODO: check why close(sockFd) if both peers close it at the same time.
    sock = check_and_cast<TcpSocket *>(socketMap.getSocketById(sockFd));
    oldSock = getSockFd(sock->getRemoteAddress());

    // Remove duplicate connections
    if (peers.count(sock->getRemoteAddress()))
    {
        EV << "Found active connection to same peer on socket fd: " << oldSock << ". Closing.\n";

        // FIXME: closing here bricks the simulation
        // if (isClient(sockFd))
        //     close(sockFd);
        return;
    }

    p = oldSock ? powPeers[oldSock] : new PowNetworkPeer;
    p->setServices(oldSock ? p->getServices() : pow::UNNAMED);
    p->setIpAddress(sock->getRemoteAddress());
    p->setPort(sock->getRemotePort());

    // Remove stale candidate on differing sockets
    // Reinserts at same index if oldSock == sockFd (peers[sockFd] = p)
    if (oldSock)
        powPeers.erase(oldSock);

    peers.insert(p->getIpAddress());
    powPeers[sockFd] = p;
    peerConnection[sockFd] = new cMessage("CONNECTION STATUS");

    self.setPort(sock->getLocalPort());

    auto response = producers["version"]->buildMessage(ictx);

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
    auto handler = consumers.find(header->getCommandName());
    IPowMsgContext ictx = {
        .msg = p,
        .peers = powPeers,
        .isClient = isClient(sockFd),
        .sockFd = sockFd,
        .self = self,
    };

    if (handler == consumers.end())
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
        // NOTE: purposely not adding a break here
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

#include "PowP2P.h"
#include "inet/common/InitStages.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/IPowMsgActions.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowPeer_m.h"
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

Define_Module(PowP2P);

void PowP2P::initialize(int stage)
{
    P2P::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
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
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        self.setServices(pow::NODE_NETWORK);
        self.setTime(time(nullptr));
        self.setIpAddress(localIp);
    }
}

void PowP2P::finish()
{
    for (auto &[_, event] : peerConnection)
        cancelAndDelete(event);

    peerConnection.clear();
    consumers.clear();
    producers.clear();
    P2P::finish();
}

void PowP2P::processSelfMessage(cMessage *msg)
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
    else
        P2P::processSelfMessage(msg);
}

void PowP2P::handleConnectReturn(int sockFd, bool connected)
{
    if (!connected)
    {
        handleConnectFailure(sockFd);
        return;
    }

    PowPeer *p;
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

    p = oldSock ? powPeers[oldSock] : new PowPeer;
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

void PowP2P::handleDataArrived(int sockFd, Packet *p)
{

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

    auto directives = handler->second->handleMessage(ictx);
    auto response = handler->second->buildResponse(ictx);

    for (const auto &d : directives)
        switch (d.action)
        {
        case OPEN:
        {
            auto data = *static_cast<ActionOpen *>(d.data);
            for (auto &p : data.peers)
            {
                p->setPort(listeningPort);
                powPeers[open(-1, SOCK_STREAM)] = p;
            }
            break;
        }
        case CANCEL:
        {
            cancelEvent(peerConnection[sockFd]);
            break;
        }
        case SCHEDULE:
        {
            auto data = *static_cast<ActionSchedule *>(d.data);
            peerConnection[sockFd]->setKind(data.eventKind);
            peerConnection[sockFd]->setContextPointer(static_cast<void *>(new int(sockFd)));
            scheduleAfter(uniform(data.eventDelayMin, data.eventDelayMax), peerConnection[sockFd]);
            break;
        }
        default:
            break;
        }

    if (response)
        _send(sockFd, response);
}

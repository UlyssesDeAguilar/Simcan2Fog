#include "PowP2PApp.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/clog.h"
#include "s2f/apps/p2p/P2PBase.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowMsgAddress_m.h"

using namespace s2f::p2p;
Define_Module(PowP2PApp);

// ------------------------------------------------------------------------- //
//                             P2PBASE OVERRIDES                             //
// ------------------------------------------------------------------------- //
void PowP2PApp::processSelfMessage(cMessage *msg)
{
    if (msg->getKind() == NODE_UP)
    {
        // TODO: NODE_UP of non-full nodes, which do not connect to the dns
        EV_INFO << "Connecting to DNS registry service" << "\n";
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

    EV_INFO << "handling connect return on peer with ip " << localIp << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    if (!connected)
    {
        handleConnectFailure(sockFd);
        return;
    }

    // TODO: NetworkPeer status enum, to handle the case where two peers try to
    //       connect to each other at the same time.
    // TODO: check why close(sockFd) if both peers close it at the same time.
    auto sock = check_and_cast<TcpSocket *>(socketMap.getSocketById(sockFd));

    // Do not connect to the same peer twice
    if (findIpInPeers(sock->getRemoteAddress()))
        return;

    // Register the peer
    PowNetworkPeer *p = new PowNetworkPeer;
    p->setIpAddress(sock->getRemoteAddress());
    p->setPort(sock->getRemotePort());
    p->setServices(pow::UNNAMED);

    peers[sockFd] = p;
    connectionQueue[sockFd];

    PowNetworkPeer self;
    self.setPort(sock->getLocalPort());
    self.setIpAddress(sock->getLocalAddress());
    self.setServices(pow::NODE_NETWORK);
    self.setTime(time(nullptr));

    auto packet = new Packet("Version message");
    auto payload = msgBuilder.buildVersionMessage(1, self, *p);
    auto header = msgBuilder.buildMessageHeader("version", payload);
    packet->insertAtBack(header);
    packet->insertAtBack(payload);
    _send(sockFd, packet);
}

bool PowP2PApp::handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort)
{
    EV << "Peer connected: " << remoteIp << ":" << remotePort << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    return true;
}

void PowP2PApp::handleDataArrived(int sockFd, Packet *p)
{

    if (sockFd == dnsSock)
    {
        P2PBase::handleDataArrived(sockFd, p);
        return;
    }

    EV_INFO << "Packet arrived from peer with connection " << sockFd << "\n";

    auto header = p->popAtFront<PowMsgHeader>();

    switch (pow::getCommand(header->getCommandName()))
    {
    case pow::VERSION:
        handleVersionMessage(sockFd, p->peekData<PowMsgVersion>());
        break;
    case pow::VERACK:
        handleVerackMessage(sockFd, header);
        break;
    case pow::GETADDR:
        handleGetaddrMessage(sockFd, header);
        break;
    case pow::ADDR:
        handleAddrMessage(sockFd, p->peekData<PowMsgAddress>());
        break;
    default:
        error("Message type not supported: %s", header->getCommandName());
    }
}

// ------------------------------------------------------------------------- //
//                             POWP2PAPP METHODS                             //
// ------------------------------------------------------------------------- //

void PowP2PApp::handleVersionMessage(int sockFd, Ptr<const PowMsgVersion> payload)
{
    auto packet = new Packet("Verack message");

    // Version missmatch - not a suitable peer
    if (payload->getVersion() != 1)
    {
        delete peers[sockFd];
        peers.erase(sockFd);
        return;
    }

    // NOTE: Temporary . Ideally, check against a list of used nonces to detect
    // a self-connection.
    if (payload->getNonce() == -1)
    {
        delete peers[sockFd];
        peers.erase(sockFd);
        return;
    }

    // Update peer data
    auto p = check_and_cast<PowNetworkPeer *>(peers[sockFd]);
    p->setServices(payload->getAddrTransServices());
    p->setTime(payload->getTimestamp());
    p->setPort(payload->getAddrTransPort());

    auto header = msgBuilder.buildMessageHeader("verack", nullptr);
    packet->insertAtBack(header);
    _send(sockFd, packet);
}

void PowP2PApp::handleVerackMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    // Only the joining node asks for new addresses
    if (!isClient(sockFd))
        return;

    auto packet = new Packet("Getaddr message");
    auto h = msgBuilder.buildMessageHeader("getaddr", nullptr);
    packet->insertAtBack(h);
    _send(sockFd, packet);
}

void PowP2PApp::handleGetaddrMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    auto packet = new Packet("Addr message");
    auto payload = msgBuilder.buildAddrMessage(reinterpret_cast<std::map<int, PowNetworkPeer *> &>(peers));
    auto h = msgBuilder.buildMessageHeader("addr", payload);
    packet->insertAtBack(h);
    packet->insertAtBack(payload);
    _send(sockFd, packet);
}

void PowP2PApp::handleAddrMessage(int sockFd, Ptr<const PowMsgAddress> payload)
{
    EV_INFO << "Received address from addr message on socket " << sockFd << "\n";
    for (int i = 0; i < payload->getIpAddressArraySize(); i++)
        EV_INFO << "    " << payload->getIpAddress(i)->getIpAddress() << "\n";
}

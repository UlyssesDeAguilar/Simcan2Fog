#include "PowP2PApp.h"
#include "omnetpp/clog.h"
#include "s2f/apps/p2p/P2PBase.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowMsgAddress_m.h"

using namespace s2f::p2p;
Define_Module(PowP2PApp);

// ------------------------------------------------------------------------- //
//                             P2PBASE OVERRIDES                             //
// ------------------------------------------------------------------------- //

void PowP2PApp::handleConnectReturn(int sockFd, bool connected)
{
    if (sockFd == dnsSock)
    {
        P2PBase::handleConnectReturn(sockFd, connected);
        return;
    }

    EV_INFO << "handling connect return on peer with ip " << localIp << "\n";

    if (!connected)
    {
        handleConnectFailure(sockFd);
        return;
    }

    auto packet = new Packet("Version message");
    auto payload = msgBuilder.buildVersionMessage();
    auto header = msgBuilder.buildMessageHeader("version", payload);
    packet->insertAtBack(header);
    packet->insertAtBack(payload);
    _send(sockFd, packet);
}

bool PowP2PApp::handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort)
{
    EV << "Peer connected: " << remoteIp << ":" << remotePort << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    PowNetworkPeer *p = new PowNetworkPeer;

    p->ipAddress = remoteIp;
    p->port = remotePort;
    peers[sockFd] = p;

    // Create the chunk queue
    connectionQueue[sockFd];
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

    switch (getCommand(header->getCommandName()))
    {
    case VERSION:
        handleVersionMessage(sockFd, p->peekData<PowMsgVersion>());
        break;
    case VERACK:
        handleVerackMessage(sockFd, header);
        break;
    case GETADDR:
        handleGetaddrMessage(sockFd, header);
        break;
    case ADDR:
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
    auto header = msgBuilder.buildMessageHeader("verack", nullptr);
    packet->insertAtBack(header);
    _send(sockFd, packet);
}

void PowP2PApp::handleVerackMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    if (localIp.str().compare("10.0.0.1") == 0)
    {
        auto packet = new Packet("Getaddr message");
        auto header = msgBuilder.buildMessageHeader("getaddr", nullptr);
        packet->insertAtBack(header);
        _send(sockFd, packet);
    }
}

void PowP2PApp::handleGetaddrMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    if (localIp.str().compare("10.0.0.4") == 0)
    {
        auto packet = new Packet("Addr message");
        auto payload = msgBuilder.buildAddrMessage();
        auto header = msgBuilder.buildMessageHeader("addr", payload);
        packet->insertAtBack(header);
        packet->insertAtBack(payload);
        _send(sockFd, packet);
    }
}

void PowP2PApp::handleAddrMessage(int sockFd, Ptr<const PowMsgAddress> payload)
{
    for (int i = 0; i < payload->getIpAddressesArraySize(); i++)
        peerCandidates.push_back(const_cast<PowNetworkPeer *>(payload->getIpAddresses(i)));
    connectToPeer();
}

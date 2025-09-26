#include "PowP2PApp.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowMsgAddress_m.h"

using namespace s2f::p2p;
Define_Module(PowP2PApp);

void PowP2PApp::handleConnectReturn(int sockFd, bool connected)
{
    EV_INFO << "handling connect return on node with ip " << localIp << "\n";
    if (!connected)
    {
        handleConnectFailure(sockFd);
        return;
    }

    _send(sockFd, msgBuilder.buildMessage(Command::VERSION));
}

void PowP2PApp::handleDataArrived(int sockFd, Packet *p)
{

    EV_INFO << "Packet arrived from client with connection " << sockFd << "\n";

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

void PowP2PApp::handleVersionMessage(int sockFd,
                                     Ptr<const PowMsgVersion> payload)
{
    _send(sockFd, msgBuilder.buildMessage(Command::VERACK));
}

void PowP2PApp::handleVerackMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    if (localIp.str().compare("10.0.0.1") == 0)
        _send(sockFd, msgBuilder.buildMessage(Command::GETADDR));
}

void PowP2PApp::handleGetaddrMessage(int sockFd, Ptr<const PowMsgHeader> header)
{
    if (localIp.str().compare("10.0.0.4") == 0)
        _send(sockFd, msgBuilder.buildMessage(Command::ADDR));
}

void PowP2PApp::handleAddrMessage(int sockFd, Ptr<const PowMsgAddress> payload)
{
    connectToPeer(payload->getIpAddresses(0).ipAddress);
}

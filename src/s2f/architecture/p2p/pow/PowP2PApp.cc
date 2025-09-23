#include "PowP2PApp.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"

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
        EV_INFO << "Version received!!\n";
        handleVersionMessage(sockFd, p->peekData<PowMsgVersion>());
        break;
    case VERACK:
        EV_INFO << "Verack received!!\n";
        break;
    }
}

void PowP2PApp::handleVersionMessage(int sockFd, Ptr<const PowMsgVersion> msg)
{

    EV_INFO << "That works, sending verack!!\n";
    _send(sockFd, msgBuilder.buildMessage(Command::VERACK));
}


#include "PowFlowManager.h"

using namespace s2f::p2p;

Define_Module(PowFlowManager);

void PowFlowManager::handleConnectReturn(int sockFd, bool connected)
{
    EV_INFO << "handling connect return.. should only run once per node\n";
    if (!connected)
    {
        handleConnectFailure();
        return;
    }

    auto packet = new Packet("Version message");
    auto data = makeShared<P2PMsg>();
    data->setCommandName("version");
    data->setPayloadSize(0);
    data->setStartString("f9beb4d9");
    data->setChecksum("test");
    packet->insertAtBack(data);
    _send(sockFd, packet);
}

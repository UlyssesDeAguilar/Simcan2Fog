#include "VersionMsgConsumer.h"

using namespace s2f::p2p::pow;
std::vector<IPowMsgDirective> VersionMsgConsumer::handleMessage(struct IPowMsgContext &ictx)
{
    auto payload = ictx.msg->peekData<Version>();
    auto p = ictx.peers[ictx.sockFd];

    // TODO: actions->ERASE to close socket and disconnect peer
    // TODO: better nonce+version verification
    if (payload->getVersion() != 1)
        return {};

    p->setServices(payload->getAddrTransServices());
    p->setTime(payload->getTimestamp());
    p->setPort(payload->getAddrTransPort());

    return {};
}

inet::Packet *VersionMsgConsumer::buildResponse(IPowMsgContext &ictx)
{
    auto packet = new inet::Packet("verack");
    packet->insertAtBack(buildHeader("verack", nullptr));
    return packet;
}

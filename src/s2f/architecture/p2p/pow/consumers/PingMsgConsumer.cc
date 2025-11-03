#include "PingMsgConsumer.h"

using namespace s2f::p2p;

inet::Packet *PingMsgConsumer::buildResponse(IPowMsgContext &ictx)
{
    inet::Packet *packet = new inet::Packet("pong");
    auto payload = inet::makeShared<PingPong>();

    payload->setNonce(ictx.msg->peekData<PingPong>()->getNonce());

    packet->insertAtBack(buildHeader("pong", nullptr));
    packet->insertAtBack(payload);

    delete ictx.msg;
    return packet;
}

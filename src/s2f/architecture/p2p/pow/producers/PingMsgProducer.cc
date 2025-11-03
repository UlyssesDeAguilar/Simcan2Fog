#include "PingMsgProducer.h"

using namespace s2f::p2p;

inet::Packet *PingMsgProducer::buildMessage(IPowMsgContext &ictx)
{
    inet::Packet *packet = new inet::Packet("ping");
    auto payload = inet::makeShared<PingPong>();

    payload->setNonce(random());

    packet->insertAtBack(buildHeader("ping", nullptr));
    packet->insertAtBack(payload);
    return packet;
}

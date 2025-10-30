#include "PingMsgProducer.h"

inet::Packet *PingMsgProducer::buildMessage(HandlerContext &ictx)
{
    inet::Packet *packet = new inet::Packet("ping");
    auto payload = inet::makeShared<PingPong>();

    payload->setNonce(random());

    packet->insertAtBack(buildHeader("ping", nullptr));
    packet->insertAtBack(payload);
    return packet;
}

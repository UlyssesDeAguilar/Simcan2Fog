#include "PingMsgConsumer.h"

HandlerResponse PingMsgConsumer::handleMessage(struct HandlerContext &ictx)
{
    return {
        .action = NOACTION,
    };
}

inet::Packet *PingMsgConsumer::buildResponse(HandlerContext &ictx)
{
    inet::Packet *packet = new inet::Packet("pong");
    auto payload = inet::makeShared<PingPong>();

    payload->setNonce(ictx.msg->peekData<PingPong>()->getNonce());

    packet->insertAtBack(buildHeader("pong", nullptr));
    packet->insertAtBack(payload);

    delete ictx.msg;
    return packet;
}

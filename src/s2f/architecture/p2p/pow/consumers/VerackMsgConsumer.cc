#include "VerackMsgConsumer.h"

HandlerResponse VerackMsgConsumer::handleMessage(struct HandlerContext &ictx)
{
    return {
        .action = SCHEDULE,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };
}

inet::Packet *VerackMsgConsumer::buildResponse(HandlerContext &ictx)
{
    if (!ictx.isClient)
        return nullptr;

    auto packet = new inet::Packet("getaddr");
    packet->insertAtBack(buildHeader("getaddr", nullptr));
    return packet;
}

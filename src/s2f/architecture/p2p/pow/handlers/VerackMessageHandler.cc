#include "VerackMessageHandler.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"

HandlerResponse VerackMessageHandler::handleMessage(struct HandlerContext &ictx)
{
    return {
        .action = SCHEDULE,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };
}

inet::Packet *VerackMessageHandler::buildResponse(HandlerContext &ictx)
{
    if (ictx.isClient == false)
        return nullptr;

    auto packet = new inet::Packet("getaddr");
    packet->insertAtBack(buildHeader("getaddr", nullptr));
    return packet;
}

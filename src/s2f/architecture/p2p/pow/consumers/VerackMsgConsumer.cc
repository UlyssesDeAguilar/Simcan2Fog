#include "VerackMsgConsumer.h"

using namespace s2f::p2p::pow;

std::vector<IPowMsgDirective> VerackMsgConsumer::handleMessage(struct IPowMsgContext &ictx)
{

    ActionSchedule *newMsg = new ActionSchedule({
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    });
    return {
        {.action = SCHEDULE, .data = static_cast<void *>(newMsg)},
    };
}

inet::Packet *VerackMsgConsumer::buildResponse(IPowMsgContext &ictx)
{
    if (!ictx.isClient)
        return nullptr;

    auto packet = new inet::Packet("getaddr");
    packet->insertAtBack(buildHeader("getaddr", nullptr));
    return packet;
}

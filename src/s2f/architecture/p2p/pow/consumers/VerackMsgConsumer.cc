#include "VerackMsgConsumer.h"

using namespace s2f::p2p;

IPowMsgResponse VerackMsgConsumer::handleMessage(struct IPowMsgContext &ictx)
{
    return {
        .action = SCHEDULE,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
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

#include "PongMsgConsumer.h"

using namespace s2f::p2p::pow;

IPowMsgResponse PongMsgConsumer::handleMessage(struct IPowMsgContext &ictx)
{
    return {
        .action = CANCEL,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };
}

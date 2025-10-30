#include "PongMsgConsumer.h"

HandlerResponse PongMsgConsumer::handleMessage(struct HandlerContext &ictx)
{
    return {
        .action = CANCEL,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };
}

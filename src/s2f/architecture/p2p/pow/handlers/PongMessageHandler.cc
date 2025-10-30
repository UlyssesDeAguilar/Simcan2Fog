#include "PongMessageHandler.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/handlers/IMessageHandler.h"

HandlerResponse PongMessageHandler::handleMessage(struct HandlerContext &ictx)
{
    return {
        .action = CANCEL,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };
}

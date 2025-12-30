#include "PongMsgConsumer.h"
#include "s2f/architecture/p2p/pow/IPowMsgActions.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"

using namespace s2f::p2p::pow;

std::vector<IPowMsgDirective> PongMsgConsumer::handleMessage(struct IPowMsgContext &ictx)
{
    ActionCancel oldMsg = {.id = ictx.sockFd};
    ActionSchedule newMsg = {
        .id = ictx.sockFd,
        .eventKind = pow::SEND_PING,
        .eventDelayMin = pow::PING_POLLING_MIN,
        .eventDelayMax = pow::PING_POLLING_MAX,
    };

    return {
        {.action = CANCEL, .data = static_cast<void *>(&oldMsg)},
        {.action = SCHEDULE, .data = static_cast<void *>(&newMsg)},
    };
}

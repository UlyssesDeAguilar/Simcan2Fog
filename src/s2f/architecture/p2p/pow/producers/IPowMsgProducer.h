#ifndef __P2P_POW_IMSGPRODUCER_H_
#define __P2P_POW_IMSGPRODUCER_H_

#include "../IPowMsgCallback.h"

/**
 * @class IMessageHandler IMessageHandler.h "IMessageHandler.h"
 *
 * Message handling interface for the pow-based p2p protocol.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
class IPowMsgProducer : public IPowMsgCallback
{

  public:
    virtual inet::Packet *buildMessage(struct HandlerContext &ctx) { return nullptr; }
};

#endif

#ifndef __P2P_POW_IMSGCONSUMER_H_
#define __P2P_POW_IMSGCONSUMER_H_

#include "../IPowMsgCallback.h"
using namespace s2f::p2p;

/**
 * @class IMessageHandler IMessageHandler.h "IMessageHandler.h"
 *
 * Message handling interface for the pow-based p2p protocol.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
class IPowMsgConsumer : public IPowMsgCallback
{
  public:
    virtual HandlerResponse handleMessage(struct HandlerContext &ictx) { return {NOACTION}; };
    virtual inet::Packet *buildResponse(struct HandlerContext &ctx) { return nullptr; }
};

#endif

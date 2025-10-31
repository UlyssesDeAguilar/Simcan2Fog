#ifndef __P2P_POW_GETADDRESSMSGCONSUMER_H__
#define __P2P_POW_GETADDRESSMSGCONSUMER_H__

#include "../messages/Address_m.h"
#include "IPowMsgConsumer.h"
using namespace s2f::p2p;

/**
 * @class GetAddressMsgConsumer GetAddressMsgConsumer.h "GetAddressMsgConsumer.h"
 *
 * Callback to consume a "getaddr" message. No action is defined.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
using namespace s2f::p2p;
class GetAddressMsgConsumer : public IPowMsgConsumer
{
  public:
    /**
     * Callback to build a response for a "getaddr" message.
     * The response is an "addr" message with peer data.
     *
     * @param ictx  Callback context.
     * @return inet::Packet representing the built response.
     */
    virtual inet::Packet *buildResponse(IPowMsgContext &ictx) override;
};

#endif

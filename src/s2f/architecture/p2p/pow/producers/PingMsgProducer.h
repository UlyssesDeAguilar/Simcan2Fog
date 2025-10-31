#ifndef __P2P_POW_PINGMSGPRODUCER_H__
#define __P2P_POW_PINGMSGPRODUCER_H__

#include "../messages/PingPong_m.h"
#include "IPowMsgProducer.h"

using namespace s2f::p2p;

/**
 * @class PingMsgProducer PingMsgProducer.h "PingMsgProducer.h"
 *
 * Producer implementation for a "ping" message.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
class PingMsgProducer : public IPowMsgProducer
{
  public:
    /**
     * Builds a "ping" message.
     * Based on messages/PingPong.msg
     *
     * @param ctx    Callback context.
     * @return the ping message.
     */
    virtual inet::Packet *buildMessage(struct IPowMsgContext &ictx) override;
};

#endif

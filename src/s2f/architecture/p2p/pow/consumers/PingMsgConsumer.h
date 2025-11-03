#ifndef __P2P_POW_PINGMSGCONSUMER_H__
#define __P2P_POW_PINGMSGCONSUMER_H__

#include "../messages/PingPong_m.h"
#include "IPowMsgConsumer.h"

namespace s2f::p2p
{
    /**
     * @class PingMsgConsumer PingMsgConsumer.h "PingMsgConsumer.h"
     *
     * Callback to consume a "ping" message. No action is defined.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class PingMsgConsumer : public IPowMsgConsumer
    {
      public:
        /**
         * Callback to build a response for a "ping" message.
         * The response is a "pong" message.
         *
         * @param ictx  Callback context.
         * @return inet::Packet representing the built response.
         */
        virtual inet::Packet *buildResponse(struct IPowMsgContext &ictx) override;
    };
}
#endif

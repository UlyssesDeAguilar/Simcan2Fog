#ifndef __P2P_POW_PONGMSGCONSUMER_H__
#define __P2P_POW_PONGMSGCONSUMER_H__

#include "../messages/PingPong_m.h"
#include "IPowMsgConsumer.h"

namespace s2f::p2p::pow
{
    /**
     * @class PongMsgConsumer PongMsgConsumer.h "PongMsgConsumer.h"
     *
     * Callback to consume a "pong" message. No response message is generated.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class PongMsgConsumer : public IPowMsgConsumer
    {
      public:
        /**
         * Callback to handle the "pong" message.
         *
         * @param ictx  Callback context.
         * @return Peer data to add.
         */
        virtual IPowMsgResponse handleMessage(IPowMsgContext &ictx) override;
    };
}
#endif

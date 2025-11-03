#ifndef __P2P_POW_IMSGPRODUCER_H__
#define __P2P_POW_IMSGPRODUCER_H__

#include "../IPowMsgCallback.h"

namespace s2f::p2p
{
    /**
     * @class IPowMsgProducer IPowMsgProducer.h "IPowMsgProducer.h"
     *
     * Callback for message producers. A producer is manually called to generate
     * an initial message (e.g., "version" and "ping") that depends on no input.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class IPowMsgProducer : public IPowMsgCallback
    {
      public:
        /**
         * Builder for the specific message.
         *
         * @param ctx    Callback context.
         * @return inet::Packet representing the built message.
         */
        virtual inet::Packet *buildMessage(struct IPowMsgContext &ctx) { return nullptr; }
    };
}

#endif

#ifndef __P2P_POW_IMSGCONSUMER_H__
#define __P2P_POW_IMSGCONSUMER_H__

#include "../IPowMsgCallback.h"
namespace s2f::p2p::pow
{
    /**
     * @class IPowMsgConsumer IPowMsgConsumer.h "IPowMsgConsumer.h"
     *
     * Callback for message consumer. A consumer is programaticlly called from a
     * message input kind and then generates the appropriate output.
     *
     * Optionally, it also derives actions to the main module.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class IPowMsgConsumer : public IPowMsgCallback
    {
      public:
        /**
         * Callback to handle a received message.
         *
         * @param ictx  Callback context.
         * @return Action context to run by the main module.
         */
        virtual std::vector<IPowMsgDirective> handleMessage(struct IPowMsgContext &ictx) { return {}; };

        /**
         * Callback to build a message response.
         *
         * @param ictx  Callback context.
         * @return inet::Packet representing the built response.
         */
        virtual inet::Packet *buildResponse(struct IPowMsgContext &ctx) { return nullptr; }
    };
}
#endif

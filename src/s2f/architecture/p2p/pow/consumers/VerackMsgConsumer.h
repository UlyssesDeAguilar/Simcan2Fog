#ifndef __P2P_POW_VERACKMSGCONSUMER_H__
#define __P2P_POW_VERACKMSGCONSUMER_H__

#include "IPowMsgConsumer.h"
namespace s2f::p2p::pow
{
    /**
     * @class VerackMsgConsumer VerackMsgConsumer.h "VerackMsgConsumer.h"
     *
     * Callback to consume a "verack" message.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class VerackMsgConsumer : public IPowMsgConsumer
    {
      public:
        /**
         * Callback to handle the "verack" message.
         *
         * @param ictx  Callback context.
         * @return Peer data to add.
         */
        virtual std::vector<IPowMsgDirective> handleMessage(struct IPowMsgContext &ictx) override;
        /**
         * Callback to build a response for a "verack" message.
         * The response is a "getaddr" message on the node that initiated the
         * connection.
         *
         * @param ictx  Callback context.
         * @return inet::Packet representing the built response.
         */
        virtual inet::Packet *buildResponse(IPowMsgContext &ictx) override;
    };
}
#endif

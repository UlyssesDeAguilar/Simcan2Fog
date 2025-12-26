#ifndef __P2P_POW_VERSIONMSGCONSUMER_H__
#define __P2P_POW_VERSIONMSGCONSUMER_H__

#include "../messages/Version_m.h"
#include "IPowMsgConsumer.h"

namespace s2f::p2p::pow
{
    /**
     * @class VersionMsgConsumer VersionMsgConsumer.h "VersionMsgConsumer.h"
     *
     * Callback to consume a "version" message.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class VersionMsgConsumer : public IPowMsgConsumer
    {
      public:
        /**
         * Callback to handle the "version" message.
         *
         * @param ictx  Callback context.
         * @return Peer data to add.
         */
        virtual IPowMsgResponse handleMessage(IPowMsgContext &ictx) override;
        /**
         * Callback to build a response for a "version" message.
         * The response is a "verack" message.
         *
         * @param ictx  Callback context.
         * @return inet::Packet representing the built response.
         */
        virtual inet::Packet *buildResponse(struct IPowMsgContext &ictx) override;
    };
}

#endif

#ifndef __P2P_POW_VERSIONMSGPRODUCER_H__
#define __P2P_POW_VERSIONMSGPRODUCER_H__

#include "../messages/Version_m.h"
#include "IPowMsgProducer.h"

namespace s2f::p2p::pow
{
    /**
     * @class VersionMsgProducer VersionMsgProducer.h "VersionMsgProducer.h"
     *
     * Producer implementation for a "version" message.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class VersionMsgProducer : public IPowMsgProducer
    {
      public:
        /**
         * Builds a "version" message.
         * Based on messages/Version.msg
         *
         * @param ctx    Callback context.
         * @return the version message.
         */
        virtual inet::Packet *buildMessage(struct IPowMsgContext &ictx) override;
    };
}
#endif

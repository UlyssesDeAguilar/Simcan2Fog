#ifndef __P2P_POW_ADDRESSMSGCONSUMER_H__
#define __P2P_POW_ADDRESSMSGCONSUMER_H__

#include "../messages/Address_m.h"
#include "IPowMsgConsumer.h"

namespace s2f::p2p::pow
{
    /**
     * @class AddressMsgConsumer AddressMsgConsumer.h "AddressMsgConsumer.h"
     *
     * Callback to consume an "addr" message. No response message is generated.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class AddressMsgConsumer : public IPowMsgConsumer
    {
      public:
        /**
         * Callback to handle the "addr" message.
         *
         * @param ictx  Callback context.
         * @return Peer data to add.
         */
        virtual IPowMsgResponse handleMessage(IPowMsgContext &ictx) override;
    };
}
#endif

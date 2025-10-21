#ifndef __POW_MESSAGE_BUILDER_H_
#define __POW_MESSAGE_BUILDER_H_

#include "PowCommon.h"
#include "PowMsgAddress_m.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/packet/chunk/FieldsChunk.h"

using namespace inet;

namespace s2f
{
    namespace p2p
    {
        /**
         * @class PowMessageBuilder PowMessageBuilder.h "PowMessageBuilder.h"
         *
         * Auxiliary class to build messages between two nodes on a PoW network.
         *
         * @author Tomás Daniel Expósito Torre
         * @date 2025-09-23
         */
        class PowMessageBuilder
        {
          public:
            /**
             * Wrapper to build a message header, which should be created after
             * the payload to append its hash value.
             *
             * @param commandName   Message kind.
             * @param payload       Message payload.
             */
            virtual Ptr<PowMsgHeader> buildMessageHeader(const char *commandName, Ptr<FieldsChunk> payload);

            /**
             * Payload builder for "version" message.
             */
            virtual Ptr<PowMsgVersion> buildVersionMessage(int version, PowNetworkPeer &self, PowNetworkPeer &peer);

            /**
             * Payload builder for "addr" message.
             */
            virtual Ptr<PowMsgAddress> buildAddrMessage();
        };
    }
}
#endif

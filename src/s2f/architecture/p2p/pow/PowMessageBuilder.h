#ifndef __POW_MESSAGE_BUILDER_H_
#define __POW_MESSAGE_BUILDER_H_

#include "PowCommon.h"
#include "inet/common/packet/Packet.h"

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
             * Wrapper method to build messages based on the message command.
             *
             * @param c     Message type.
             */
            virtual Packet *buildMessage(enum Command c);

          protected:
            /**
             * Builds the "version" message, exchanged at the beginning of the
             * the connection between two nodes.
             */
            virtual Packet *buildVersionMessage();

            /**
             * Builds the "verack" message, exchanged when network versions
             * match for two connecting nodes.
             */
            virtual Packet *buildVerackMessage();

            /**
             * Builds the "getaddr" message, sent by the transmitting node to
             * discover new peer candidates.
             */
            virtual Packet *buildGetaddrMessage();

            /**
             * Builds the "addr" message, sent by the receiving node in answer
             * to an addr message.
             */
            virtual Packet *buildAddrMessage();
        };
    }
}
#endif

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
         * Auxiliary class to build messages between two nodes on a PoW network.
         */
        class PowMessageBuilder
        {
          public:
            /**
             * Wrapper method to build messages based on the message command.
             *
             * @param
             */
            virtual Packet *buildMessage(enum Command c);

          protected:
            virtual Packet *buildVersionMessage();
            virtual Packet *buildVerackMessage();
        };
    }
}
#endif

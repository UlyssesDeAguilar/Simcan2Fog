#ifndef __POW_FLOW_MANAGER_H_
#define __POW_FLOW_MANAGER_H_

#include "PowCommon.h"
#include "PowMessageBuilder.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include "inet/common/Ptr.h"
#include "s2f/apps/p2p/P2PBase.h"

namespace s2f
{
    namespace p2p
    {
        /**
         * @class PowP2PApp PowP2PApp.h "PowP2PApp.h"
         *
         * Peer-to-peer protocol specification for nodes managing a proof of
         * work based blockchain, inspired by bitcoin.
         *
         * @author Tomás Daniel Expósito Torre
         * @date 2025-09-23
         */
        class PowP2PApp : public P2PBase
        {
          protected:
            PowMessageBuilder msgBuilder; //<! Object to build messages

            // ------------------------------------------------------------- //
            //                       P2PBASE OVERRIDES                       //
            // ------------------------------------------------------------- //

            /**
             * Handles the initial connection to another peer candidate in the
             * network.
             *
             * @param sockFd    connection file descriptor.
             * @param connected connection status.
             */
            virtual void handleConnectReturn(int sockFd,
                                             bool connected) override;

            /**
             * Handles packets arrived from an existing connection.
             *
             * @param sockFd    connection file descriptor.
             * @param p         connection incoming data.
             */
            virtual void handleDataArrived(int sockFd, Packet *p) override;

            // -------------------------------------------------------------- //
            //                       MSGBUILDER METHODS                       //
            // -------------------------------------------------------------- //

            /**
             * Handles an incoming "version" message.
             *
             * @param sockFd    connection file descriptor.
             * @param msg       message payload.
             */
            virtual void handleVersionMessage(int sockFd,
                                              Ptr<const PowMsgVersion> msg);
        };
    }
};
#endif

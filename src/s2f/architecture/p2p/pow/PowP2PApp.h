#ifndef __POW_FLOW_MANAGER_H_
#define __POW_FLOW_MANAGER_H_

#include "PowCommon.h"
#include "PowMessageBuilder.h"
#include "handlers/IMessageHandler.h"
#include "inet/common/Ptr.h"
#include "s2f/apps/p2p/P2PBase.h"
#include "s2f/architecture/p2p/pow/PowNetworkPeer_m.h"

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
            PowMessageBuilder msgBuilder;                      //!< Object to build messages
            std::map<int, cMessage *> peerConnection;          //!< Peer event handlers
            std::map<std::string, IMessageHandler *> handlers; //!< Message handlers
            std::map<int, PowNetworkPeer *> &powPeers =
                reinterpret_cast<std::map<int, PowNetworkPeer *> &>(peers); //!< Peer list in PowNetworkPeer format

            // ------------------------------------------------------------- //
            //                       P2PBASE OVERRIDES                       //
            // ------------------------------------------------------------- //

            /**
             * Initialization hook for this module.
             */
            virtual void initialize() override;

            /**
             * Finish hook that runs when the simulation is terminated without errors.
             */
            virtual void finish() override;

            /**
             * Handle hook for messages sent by this module.
             *
             * @param msg   Message to process.
             */
            virtual void processSelfMessage(cMessage *msg) override;

            /**
             * Handles the initial connection to another peer candidate in the
             * network.
             *
             * @param sockFd    connection file descriptor.
             * @param connected connection status.
             */
            virtual void handleConnectReturn(int sockFd, bool connected) override;

            /**
             * Handles packets arrived from an existing connection.
             *
             * @param sockFd    connection file descriptor.
             * @param p         connection incoming data.
             */
            virtual void handleDataArrived(int sockFd, Packet *p) override;

            /**
             * Handle hook for socket connection initiated by a possible peer.
             *
             * @param sockFd    File descriptor for this connection.
             * @param remoteIp  Peer Ip address.
             */
            virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort) override;
        };
    }
};
#endif

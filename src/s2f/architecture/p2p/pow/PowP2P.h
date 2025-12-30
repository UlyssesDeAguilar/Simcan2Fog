#ifndef __P2P_POW_APP_H__
#define __P2P_POW_APP_H__

#include "PowCommon.h"
#include "consumers/IPowMsgConsumer.h"
#include "inet/common/Ptr.h"
#include "producers/IPowMsgProducer.h"
#include "s2f/apps/p2p/P2P.h"
#include "s2f/architecture/p2p/pow/PowPeer_m.h"
#include <memory>

namespace s2f::p2p::pow
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
    class PowP2P : public P2P
    {
      protected:
        PowPeer self;                                                                                //!< Representation of this node
        std::map<int, cMessage *> peerConnection;                                                    //!< Peer event handlers
        std::map<std::string, std::unique_ptr<IPowMsgConsumer>> consumers;                           //!< Message handlers
        std::map<std::string, std::unique_ptr<IPowMsgProducer>> producers;                           //!< Message handlers
        std::map<int, PowPeer *> &powPeers = reinterpret_cast<std::map<int, PowPeer *> &>(peerData); //!< Peer list in PowNetworkPeer format
        cGate *node{};                                                                               //!< Processing node, SPV client, etc

        // ------------------------------------------------------------- //
        //                           OVERRIDES                           //
        // ------------------------------------------------------------- //

        /**
         * Initialization hook for this module.
         */
        virtual void initialize(int stage) override;

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
    };
}
#endif

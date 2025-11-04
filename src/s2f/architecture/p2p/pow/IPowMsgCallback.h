#ifndef __P2P_POW_IMSGCALLBACK_H__
#define __P2P_POW_IMSGCALLBACK_H__

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowNetworkPeer_m.h"
#include "s2f/architecture/p2p/pow/messages/Header_m.h"
#include <omnetpp.h>
#include <vector>

namespace s2f::p2p::pow
{
    /**
     * Delegated action to the module. Used for omnet-dependant procedures which
     * cannot be delegated to other classes.
     */
    enum IPowMsgAction
    {
        OPEN,     //!< Open sockets for incoming peers
        SCHEDULE, //!< Schedule an event for a peer
        CANCEL,   //!< Cancel an event for a peer
        NOACTION
    };

    /**
     * Information derived from callback to use in the main module.
     */
    struct IPowMsgResponse
    {
        enum IPowMsgAction action;           //<! Action to execute
        short eventKind;                     //<! Event type to schedule
        int eventDelayMin;                   //<! Minimum delay for self event
        int eventDelayMax;                   //<! Maximum delay for self event
        std::vector<PowNetworkPeer *> peers; //<! New peers to connect to
    };

    /**
     * Information required by callback to process messages.
     * Peer data is subject to modification by the callback.
     */
    struct IPowMsgContext
    {
        inet::Packet *msg;                      //!< Received message
        std::map<int, PowNetworkPeer *> &peers; //<! All known peers
        bool isClient;                          //!< For messages only initiated by one peer
        int sockFd;                             //!< Active connection
        PowNetworkPeer &self;                   //!< Caller information
    };

    /**
     * @class IPowMsgCallback IPowMsgCallback.h "IPowMsgCallback.h"
     *
     * Message processing callback for the pow-based p2p protocol.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-10-29
     */
    class IPowMsgCallback
    {
      protected:
        /**
         * Builds a packet header. This method should be called after building the
         * packet payload.
         *
         * @param name      Targetted consumer implementation.
         * @param payload   Packet payload to compute the checksum.
         *
         * @return The built header.
         */
        virtual inet::Ptr<Header> buildHeader(const char *name, inet::Ptr<inet::FieldsChunk> payload)
        {
            auto header = inet::makeShared<Header>();

            // Message Header
            header->setCommandName(name);
            header->setStartString(pow::MAIN_NET);

            // TODO: determine size from payload and sha256 to create a checksum
            header->setPayloadSize(0);
            header->setChecksum("TODO");

            return header;
        }

      public:
        virtual ~IPowMsgCallback() = default;
    };
}
#endif

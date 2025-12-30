#ifndef __P2P_POW_IMSGCALLBACK_H__
#define __P2P_POW_IMSGCALLBACK_H__

#include "IPowMsgActions.h"
#include "inet/common/Units.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowPeer_m.h"
#include "s2f/architecture/p2p/pow/messages/Header_m.h"
#include "s2f/os/crypto/base.h"
#include "s2f/os/crypto/hash.h"
#include <omnetpp.h>
#include <vector>

namespace s2f::p2p::pow
{

    struct IPowMsgDirective
    {
        enum IPowMsgAction action; //<! Action to execute
        void *data;                //<! Data required by action
    };

    /**
     * Information required by callback to process messages.
     * Peer data is subject to modification by the callback.
     */
    struct IPowMsgContext
    {
        inet::Packet *msg;               //!< Received message
        std::map<int, PowPeer *> &peers; //<! All known peers
        bool isClient;                   //!< For messages only initiated by one peer
        int sockFd;                      //!< Active connection
        PowPeer &self;                   //!< Caller information
    };

    /**
     * @class IPowMsgCallback IPowMsgCallback.h "IPowMsgCallback.h"
     *
     * Message processing callback interface for the pow-based p2p protocol.
     *
     * Every message kind should be associated with one callback, which operates over
     * it and sends one or more actions the base module has to follow.
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
            int byteSize = payload ? inet::units::values::B(payload->getChunkLength()).get() : 0;

            // Message Header
            header->setCommandName(name);
            header->setStartString(pow::MAIN_NET);

            // TODO: determine size from payload and sha256 to create a checksum
            header->setPayloadSize(byteSize);
            header->setChecksum("TODO");

            return header;
        }

      public:
        virtual ~IPowMsgCallback() = default;
    };
}
#endif

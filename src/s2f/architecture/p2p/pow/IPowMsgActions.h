#ifndef __P2P_POW_IMSGACTIONS_H__
#define __P2P_POW_IMSGACTIONS_H__

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
    /**
     * Delegated action to the module. Used for omnet-dependant procedures which
     * cannot be delegated to other classes.
     */
    enum IPowMsgAction
    {
        OPEN,     //!< Open sockets for incoming peers
        SCHEDULE, //!< Schedule an event for a peer
        CANCEL,   //!< Cancel an event for a peer
        DELEGATE, //!< Delegate to processing node
    };

    struct ActionOpen
    {
        std::vector<PowPeer *> peers; //<! New peers to connect to
    };

    struct ActionSchedule
    {
        short eventKind;   //<! Event type to schedule
        int eventDelayMin; //<! Minimum delay for self event
        int eventDelayMax; //<! Maximum delay for self event
    };
}
#endif

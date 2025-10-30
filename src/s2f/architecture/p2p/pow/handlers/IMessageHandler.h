#ifndef __P2P_IMESSAGEHANDLER_H_
#define __P2P_IMESSAGEHANDLER_H_

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/cmessage.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowNetworkPeer_m.h"
#include "s2f/architecture/p2p/pow/messages/Header_m.h"
#include <omnetpp.h>
#include <vector>

using namespace s2f::p2p;

/**
 * Delegated action to the module. Used for omnet-dependant procedures which
 * cannot be delegated to other classes.
 */
enum HandlerAction
{
    OPEN,     //!< Open sockets for incoming peers
    SCHEDULE, //!< Schedule an event for a peer
    CANCEL,   //!< Cancel an event for a peer
    NOACTION
};

struct HandlerResponse
{
    enum HandlerAction action;
    std::vector<PowNetworkPeer *> peers;
};

struct HandlerContext
{
    std::map<int, PowNetworkPeer *> &peers; //<! All known peers
    bool isClient;                          //!< For messages only initiated by one peer
    int sockFd;                             //!< Active connection
    PowNetworkPeer &self;
    inet::L3Address localIp;
};

/**
 * @class IMessageHandler IMessageHandler.h "IMessageHandler.h"
 *
 * Message handling interface for the pow-based p2p protocol.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
class IMessageHandler
{
  protected:
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
    virtual struct HandlerResponse handleMessage(inet::Packet *msg, struct HandlerContext &ictx) { return {NOACTION}; };
    virtual inet::Packet *buildResponse(struct HandlerContext &ctx) { return nullptr; }
    virtual ~IMessageHandler() = default;
};

#endif

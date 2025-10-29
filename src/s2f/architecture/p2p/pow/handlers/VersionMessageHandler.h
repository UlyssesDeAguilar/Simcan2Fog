#ifndef __P2P_VERSIONMESSAGEHANDLER_H_
#define __P2P_VERSIONMESSAGEHANDLER_H_

#include "../messages/Version_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;

class VersionMessageHandler : public IMessageHandler
{
  public:
    virtual struct HandlerResponse handleMessage(inet::Packet *msg, HandlerContext &ictx) override;
    virtual inet::Packet *buildResponse(struct HandlerContext &ictx) override;
};

#endif

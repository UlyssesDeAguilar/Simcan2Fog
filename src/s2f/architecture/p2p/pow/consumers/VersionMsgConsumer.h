#ifndef __P2P_VERSIONMSGCONSUMER_H_
#define __P2P_VERSIONMSGCONSUMER_H_

#include "../messages/Version_m.h"
#include "IPowMsgConsumer.h"

using namespace s2f::p2p;

class VersionMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
    virtual inet::Packet *buildResponse(struct HandlerContext &ictx) override;
};

#endif

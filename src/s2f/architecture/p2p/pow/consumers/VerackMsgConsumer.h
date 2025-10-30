#ifndef __P2P_VERACKMSGCONSUMER_H
#define __P2P_VERACKMSGCONSUMER_H

#include "IPowMsgConsumer.h"

using namespace s2f::p2p;
class VerackMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual HandlerResponse handleMessage(struct HandlerContext &ictx) override;
    virtual inet::Packet *buildResponse(HandlerContext &ictx) override;
};

#endif

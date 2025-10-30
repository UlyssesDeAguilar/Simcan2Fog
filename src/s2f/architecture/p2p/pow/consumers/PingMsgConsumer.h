#ifndef __P2P_PINGMSGHANDLER_H_
#define __P2P_PINGMSGHANDLER_H_

#include "../messages/PingPong_m.h"
#include "IPowMsgConsumer.h"

using namespace s2f::p2p;

class PingMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
    virtual inet::Packet *buildResponse(struct HandlerContext &ictx) override;
};

#endif

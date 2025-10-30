#ifndef __P2P_PONGMSGCONSUMER_H_
#define __P2P_PONGMSGCONSUMER_H_

#include "../messages/PingPong_m.h"
#include "IPowMsgConsumer.h"

using namespace s2f::p2p;

class PongMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
};

#endif

#ifndef __P2P_PINGMSGPRODUCER_H_
#define __P2P_PINGMSGPRODUCER_H_

#include "../messages/PingPong_m.h"
#include "IPowMsgProducer.h"

using namespace s2f::p2p;

class PingMsgProducer : public IPowMsgProducer
{

  public:
    virtual inet::Packet *buildMessage(struct HandlerContext &ictx) override;
};

#endif

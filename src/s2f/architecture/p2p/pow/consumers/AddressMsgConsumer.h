#ifndef __P2P_ADDRESSMSGCONSUMER_H_
#define __P2P_ADDRESSMSGCONSUMER_H_

#include "../messages/Address_m.h"
#include "IPowMsgConsumer.h"

using namespace s2f::p2p;
class AddressMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
};

#endif

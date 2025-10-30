#ifndef __P2P_GETADDRESSMSGCONSUMER_H_
#define __P2P_GETADDRESSMSGCONSUMER_H_

#include "../messages/Address_m.h"
#include "IPowMsgConsumer.h"

using namespace s2f::p2p;
class GetAddressMsgConsumer : public IPowMsgConsumer
{
  public:
    virtual inet::Packet *buildResponse(HandlerContext &ictx) override;
};

#endif

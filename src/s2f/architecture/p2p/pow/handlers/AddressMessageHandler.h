#ifndef __P2P_VERACKMESSAGEHANDLER_H_
#define __P2P_VERACKMESSAGEHANDLER_H_

#include "../messages/Address_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;
class AddressMessageHandler : public IMessageHandler
{
  public:
    virtual HandlerResponse handleMessage(inet::Packet *msg, HandlerContext &ictx) override;
};

#endif

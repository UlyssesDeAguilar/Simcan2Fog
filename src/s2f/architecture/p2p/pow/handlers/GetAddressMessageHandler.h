#ifndef __P2P_GETADDRESSMESSAGEHANDLER_H_
#define __P2P_GETADDRESSMESSAGEHANDLER_H_

#include "../messages/Address_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;
class GetAddressMessageHandler : public IMessageHandler
{
  public:
    virtual inet::Packet *buildResponse(HandlerContext &ictx) override;
};

#endif

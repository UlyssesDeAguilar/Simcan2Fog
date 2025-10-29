#ifndef __P2P_VERACKMESSAGEHANDLER_H_
#define __P2P_VERACKMESSAGEHANDLER_H_

#include "IMessageHandler.h"

using namespace s2f::p2p;
class VerackMessageHandler : public IMessageHandler
{
  public:
    virtual inet::Packet *buildResponse(HandlerContext &ictx) override;
};

#endif

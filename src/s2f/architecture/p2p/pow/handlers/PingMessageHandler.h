#ifndef __P2P_PINGMESSAHEHANDLER_H_
#define __P2P_PINGMESSAHEHANDLER_H_

#include "../messages/PingPong_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;

class PingMessageHandler : public IMessageHandler
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
    virtual inet::Packet *buildResponse(struct HandlerContext &ictx) override;
};

#endif

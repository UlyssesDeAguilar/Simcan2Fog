#ifndef __P2P_PONGMESSAHEHANDLER_H_
#define __P2P_PONGMESSAHEHANDLER_H_

#include "../messages/PingPong_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;

class PongMessageHandler : public IMessageHandler
{
  public:
    virtual HandlerResponse handleMessage(HandlerContext &ictx) override;
};

#endif

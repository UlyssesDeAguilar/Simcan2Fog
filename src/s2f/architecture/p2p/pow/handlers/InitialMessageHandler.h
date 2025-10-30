#ifndef __P2P_INITIALMESSAGEHANDLER_H_
#define __P2P_INITIALMESSAGEHANDLER_H_

#include "../messages/Version_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;

class InitialMessageHandler : public IMessageHandler
{
  public:
    virtual inet::Packet *buildResponse(struct HandlerContext &ictx) override;
};

#endif

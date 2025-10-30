#ifndef __P2P_INITIALMESSAGEBUILDER_H_
#define __P2P_INITIALMESSAGEBUILDER_H_

#include "../messages/PingPong_m.h"
#include "../messages/Version_m.h"
#include "IMessageHandler.h"

using namespace s2f::p2p;

class InitialMessageBuilder : public IMessageHandler
{
  private:
    inet::Packet *buildVersion(struct HandlerContext &ictx);
    inet::Packet *buildPing(struct HandlerContext &ictx);

  public:
    virtual inet::Packet *buildMessage(std::string_view command, struct HandlerContext &ictx)
    {
        if (command == "version")
        {
            return buildVersion(ictx);
        }
        else if (command == "ping")
        {
            return buildPing(ictx);
        }
        return nullptr;
    }
};

#endif

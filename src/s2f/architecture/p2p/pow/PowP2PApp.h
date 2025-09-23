#ifndef __POW_FLOW_MANAGER_H_
#define __POW_FLOW_MANAGER_H_

#include "PowCommon.h"
#include "PowMessageBuilder.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include "inet/common/Ptr.h"
#include "s2f/apps/p2p/P2PBase.h"

namespace s2f
{
    namespace p2p
    {
        class PowP2PApp : public P2PBase
        {
          protected:
            PowMessageBuilder msgBuilder;

            virtual void handleConnectReturn(int sockFd,
                                             bool connected) override;

            virtual void handleDataArrived(int sockFd, Packet *p) override;
            virtual void handleVersionMessage(int sockFd,
                                              Ptr<const PowMsgVersion> msg);
        };
    }
};
#endif

#ifndef __POW_FLOW_MANAGER_H_
#define __POW_FLOW_MANAGER_H_

#include "s2f/apps/p2p/P2PBase.h"

namespace s2f
{
    namespace p2p
    {
        class PowFlowManager : public P2PBase
        {
          protected:
            virtual void handleConnectReturn(int sockFd,
                                             bool connected) override;
        };
    }
};
#endif

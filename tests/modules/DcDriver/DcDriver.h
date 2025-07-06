#include "s2f/core/include/SIMCAN_types.h"
#include "s2f/core/cSIMCAN_Core.h"
// #include "s2f/architecture/net/routing/RoutingInfo_m.h"
#include "s2f/messages/SM_UserVM.h"

namespace s2f::tests
{
    class DcDriver : public cSIMCAN_Core
    {
    private:
        //GlobalAddress dcAddress;
        SM_UserVM *request; // Scale this
    protected:
        virtual void initialize() override;
        virtual void finish() override;
        virtual cGate *getOutGate(cMessage *sm) override;
        virtual void processRequestMessage(SIMCAN_Message *sm) override;
        virtual void processResponseMessage(SIMCAN_Message *sm) override;
        virtual void processSelfMessage(cMessage *msg) override;
    };
}

#include "Core/include/SIMCAN_types.h"
#include "Core/cSIMCAN_Core.h"
#include "Architecture/Network/RoutingInfo/RoutingInfo_m.h"
#include "Messages/SM_UserVM.h"

class DcDriver : public cSIMCAN_Core
{
private:
    GlobalAddress dcAddress;
    SM_UserVM *request;         // Scale this
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual cGate* getOutGate(cMessage *sm) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
    virtual void processSelfMessage(cMessage *msg) override;
};
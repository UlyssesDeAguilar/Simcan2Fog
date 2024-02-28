#include "Core/include/SIMCAN_types.h"
#include "Core/cSIMCAN_Core.h"
#include "Architecture/Network/RoutingInfo/RoutingInfo_m.h"
#include "Messages/SM_UserVM.h"

class DcDriver : public omnetpp::cSimpleModule
{
private:
    GlobalAddress dcAddress;
    SM_UserVM *request;         // Scale this
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
};
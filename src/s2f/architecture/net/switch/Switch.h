#ifndef SIMCAN_EX_SWITCH_H_
#define SIMCAN_EX_SWITCH_H_

#include "../../../core/cSIMCAN_Core.h"
#include "../routing/RoutingInfo_m.h"

class Switch : public cSimpleModule
{
protected:
    GateInfo manager; //!< Info for manger
    GateInfo network; //!< Info for networking
    GateInfo lower;   //!< Used for indexing

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif

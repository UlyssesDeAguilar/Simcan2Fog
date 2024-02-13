#ifndef __SIMCAN_2_0_SWITCH_H_
#define __SIMCAN_2_0_SWITCH_H_

#include "Core/cSIMCAN_Core.h"
#include "Architecture/Network/RoutingInfo/RoutingInfo_m.h"


class Switch : public cSIMCAN_Core
{
protected:
    GateInfo upper;   //!< Info of upper gate
    GateInfo lower;   //!< Used for indexing

    virtual void initialize() override;
    void finish() override;
    cGate *getOutGate(cMessage *msg) override;
    void processSelfMessage(cMessage *msg) override { error("This module cannot process self messages: %s", msg->getName()); }
    void processRequestMessage(SIMCAN_Message *sm) override;
    void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif

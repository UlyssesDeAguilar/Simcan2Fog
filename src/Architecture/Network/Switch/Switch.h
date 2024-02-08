#ifndef __SIMCAN_2_0_SWITCH_H_
#define __SIMCAN_2_0_SWITCH_H_

#include "Core/cSIMCAN_Core.h"

/**
 * TODO: Implement newer version and avoid gate arrays (use indexing instead)
 */
class Switch : public cSIMCAN_Core
{

protected:
    cGate **inputGates;
    cGate **outputGates;
    std::string type;

    virtual void initialize() override;
    void finish() override;

private:
    cGate *getOutGate(cMessage *msg) override;
    void processSelfMessage(cMessage *msg) override;
    void processRequestMessage(SIMCAN_Message *sm) override;
    void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif

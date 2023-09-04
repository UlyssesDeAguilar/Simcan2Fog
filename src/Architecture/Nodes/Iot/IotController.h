#ifndef __SIMCAN_2_0_IOT_CONTROLLER
#define __SIMCAN_2_0_IOT_CONTROLLER

#include <omnetpp.h>                 // The framework core
#include "Core/cSIMCAN_Core.h"       // Core of the SIMCAN simulator
#include "Messages/SIMCAN_Message.h" // Base for the SIMCAN messages
#include "Messages/SM_FogIO.h"       // Message that models IO requests to an FogNode

class IotController : public cSIMCAN_Core
{
private:
    SM_FogIO *fogRequest;
    unsigned short updateCounter;      // The number of times it was updated
protected:
    SM_FogIO* createTestFogIORequest(); // FIXME Will be deleted
    virtual void initialize() override;
    virtual void finish() override;
    virtual cGate *getOutGate(cMessage *msg) override;
    virtual void processSelfMessage(cMessage *msg) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
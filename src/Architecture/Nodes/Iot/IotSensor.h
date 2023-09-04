#ifndef __SIMCAN_2_0_IOT_SENSOR
#define __SIMCAN_2_0_IOT_SENSOR

#include <omnetpp.h>                 // The framework core
#include "Core/cSIMCAN_Core.h"       // Core of the SIMCAN simulator
#include "Messages/SIMCAN_Message.h" // Base for the SIMCAN messages
#include "Messages/SM_FogIO.h"       // Message that models IO requests to an FogNode

class IotSensor : public cSIMCAN_Core
{
private:
    double updatePeriod;
    SIMCAN_Message *ping;
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual cGate *getOutGate(cMessage *msg) override;
    virtual void processSelfMessage(cMessage *msg) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif

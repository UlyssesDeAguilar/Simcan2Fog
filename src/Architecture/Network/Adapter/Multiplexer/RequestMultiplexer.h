#ifndef SIMCAN_EX_REQUEST_MULTIPLEXER
#define SIMCAN_EX_REQUEST_MULTIPLEXER

#include <omnetpp.h>                 // The framework core
#include "Core/cSIMCAN_Core.h"       // Core of the SIMCAN simulator
#include "Messages/SIMCAN_Message.h" // Base for the SIMCAN messages
#include "Messages/SM_ResolverRequest_m.h"

class RequestMultiplexer : public cSimpleModule
{
private:
    cGate * outGate;
    int gInputId;
protected:
    virtual void initialize() override;
    virtual void finish() override;
    //virtual cGate *getOutGate(cMessage *msg) override;

    virtual void handleMessage(cMessage *msg) override;
    //virtual void processSelfMessage(cMessage *msg) override;
    //virtual void processRequestMessage(SIMCAN_Message *sm) override;
    //virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
#ifndef __SIMCAN_2_0_STORAGENETWORK_H_
#define __SIMCAN_2_0_STORAGENETWORK_H_

#include "Core/cSIMCAN_Core.h"

/**
 * TODO: Consider if this element stays 
 */
class StorageNetwork : public cSIMCAN_Core
{
protected:
    cGate **inputGates;
    cGate **outputGates;
    cGate *fromCommGate;
    cGate *toCommGate;

    virtual void initialize() override;
    virtual void finish() override;

private:

    cGate *getOutGate(cMessage *msg) override;
    void processSelfMessage(cMessage *msg) override;
    void processRequestMessage(SIMCAN_Message *sm) override;
    void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif

#ifndef __SIMCAN_2_0_FOG_NODE
#define __SIMCAN_2_0_FOG_NODE

#include <omnetpp.h>                    // The framework core
#include "Core/cSIMCAN_Core.h"          // Core of the SIMCAN simulator
#include "Messages/SIMCAN_Message.h"    // Base for the SIMCAN messages

/*
 * For stubbing the request of an User app deployed in this FogNode
 * In the future it will point to an actual simulated user !
 */
#include "Messages/SM_UserVM.h"                         // Message for requesting a VM or VMs
#include "Messages/SM_UserAPP.h"                        // Message for requesting the execution of an app or apps

class FogNode: public cSIMCAN_Core {
private:
    cGate * outGate;
    cMessage * event;
    SM_UserVM* vmsRequest;
    SM_UserAPP* appsRequest;
    SM_UserVM* createVmTestRequest();       // FIXME Will be deleted
    SM_UserAPP* createAppTestRequest();     // FIXME Will be deleted
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual cGate * getOutGate(cMessage *msg) override;
    virtual void processSelfMessage(cMessage *msg) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif

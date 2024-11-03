#include "s2f/core/include/SIMCAN_types.h"
#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/messages/SIMCAN_Message.h"

class MQDriver : public cSIMCAN_Core
{
protected:
    virtual void initialize() override;
    virtual void finish() override {};
    virtual cGate* getOutGate(cMessage *sm) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
    virtual void processSelfMessage(cMessage *msg) override;
};
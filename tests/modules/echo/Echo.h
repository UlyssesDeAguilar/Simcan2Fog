#include "s2f/core/include/SIMCAN_types.h"
#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/architecture/net/routing/RoutingInfo_m.h"

using namespace inet;

class Echo : public cSIMCAN_Core
{
private:
    bool recv;           //!< Recieve or Send mode
    int ip;              //!< Ip of the node
    int targetIp;        //!< Initial target IP address
    ServiceURL localUrl; //!< The resource identifier for the node
    int addressRange;    //!< Address range to test (sequential range based on base IP)
    int numAcks;
    SIMCAN_Message *base; //!< Message template

protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual cGate *getOutGate(cMessage *msg) override;
    virtual void processSelfMessage(cMessage *msg) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
};
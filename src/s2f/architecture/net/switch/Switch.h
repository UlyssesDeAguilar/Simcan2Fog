#ifndef SIMCAN_EX_SWITCH_H_
#define SIMCAN_EX_SWITCH_H_

#include <omnetpp.h>
#include "s2f/architecture/net/protocol/L2Protocol_m.h"

class Switch : public omnetpp::cSimpleModule
{
protected:
    std::map<int, int> commutationTable;

    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
};

#endif

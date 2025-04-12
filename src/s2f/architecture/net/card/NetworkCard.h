#ifndef SIMCAN_EX_NETWORKCARD_H__
#define SIMCAN_EX_NETWORKCARD_H__

#include <omnetpp.h>
#include "s2f/architecture/net/protocol/L2Protocol_m.h"

class NetworkCard : public omnetpp::cSimpleModule
{
protected:
    int macAddress;

    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
};

#endif /* SIMCAN_EX_NETWORKCARD_H__ */
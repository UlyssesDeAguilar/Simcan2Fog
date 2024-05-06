#ifndef SIMCAN_EX_STACK_MULTIPLEXER_H
#define SIMCAN_EX_STACK_MULTIPLEXER_H

#include <array>
#include <omnetpp.h>
#include "Messages/SIMCAN_Message.h"
#include "StackServiceType.h"

/**
 * @brief Common interface for registered services
 */
class StackService {
    public:
        virtual void processRequest(omnetpp::cMessage *msg) = 0;
};

/**
 * @brief Mission: dispatch messages to the corresponding services
 */
class StackMultiplexer : public omnetpp::cSimpleModule
{
public:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    void processResponse(omnetpp::cMessage *msg);
private:
    std::array<StackService *, StackServiceType::NUM_SERVICES> services;
    int outGateId;
};

#endif /*SIMCAN_EX_STACK_MULTIPLEXER_H*/

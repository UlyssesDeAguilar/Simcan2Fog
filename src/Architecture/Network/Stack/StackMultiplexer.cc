#include "StackMultiplexer.h"

using namespace omnetpp;

Define_Module(StackMultiplexer);

void StackMultiplexer::initialize()
{
    // Get the out gate id
    outGateId = gateHalf("comm", cGate::OUTPUT)->getId();

    // Locate and get references to the services
    services[StackServiceType::HTTP_CLIENT] = check_and_cast<StackService *>(getModuleByPath("^.app[0]"));
    services[StackServiceType::HTTP_PROXY] = check_and_cast<StackService *>(getModuleByPath("^.app[1]"));
    services[StackServiceType::DNS_RESOLVER] = check_and_cast<StackService *>(getModuleByPath("^.app[2]"));
}

void StackMultiplexer::finish() {}

void StackMultiplexer::handleMessage(omnetpp::cMessage *msg)
{
    // Drop ownership -- delegation to the services
    drop(msg);
    // Recover recuested service
    int16_t serviceType = msg->getKind();
    // Sanity check
    assert(msg->getKind() < StackServiceType::NUM_SERVICES && msg->getKind() >= 0);
    // Dispatch
    services[serviceType]->processRequest(msg);
}

void StackMultiplexer::processResponse(omnetpp::cMessage *msg)
{
    Enter_Method_Silent();
    take(msg);
    // Relay the response
    send(msg, outGateId);
}
#include "MQDriver.h"
using namespace s2f::tests;

Define_Module(MQDriver);

void MQDriver::initialize()
{
    destinationTopic = par("destinationTopic");
    if(!opp_isempty(destinationTopic))
        scheduleAt(simTime(), new SIMCAN_Message("Request"));
    
    cSIMCAN_Core::initialize();
}

cGate *MQDriver::getOutGate(cMessage *sm)
{
    return gate("out");
}

void MQDriver::processSelfMessage(cMessage *msg)
{
    EV << "Kick start, publishing to " << destinationTopic << "\n";
    auto sm = reinterpret_cast<SIMCAN_Message *>(msg);
    sm->setDestinationTopic(destinationTopic);
    sendRequestMessage(sm, gate("out"));
}

void MQDriver::processResponseMessage(SIMCAN_Message *sm)
{
    EV << "Recieved response\n";
    delete sm;
}

void MQDriver::processRequestMessage(SIMCAN_Message *sm)
{
    EV << "Recieved request, sending back as response\n";
    sm->setIsResponse(true);
    sm->setDestinationTopic(sm->getReturnTopic());
    sendResponseMessage(sm);
}
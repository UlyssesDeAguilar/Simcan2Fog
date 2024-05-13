#include "MQDriver.h"

Define_Module(MQDriver);

void MQDriver::initialize()
{
    if((bool) par("kickStart"))
        scheduleAt(simTime(), new SIMCAN_Message("Request"));
    
    cSIMCAN_Core::initialize();
}

cGate *MQDriver::getOutGate(cMessage *sm)
{
    return gate("out");
}

void MQDriver::processSelfMessage(cMessage *msg)
{
    const char *topic = getModuleByPath("<root>")->par("endPointTopic");
    EV << "Kick start, publishing to " << topic;
    auto sm = reinterpret_cast<SIMCAN_Message *>(msg);
    sm->setDestinationTopic(topic);
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
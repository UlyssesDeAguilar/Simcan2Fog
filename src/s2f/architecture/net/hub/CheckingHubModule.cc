#include "../hub/CheckingHubModule.h"

Define_Module(CheckingHubModule);

void CheckingHubModule::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init module parameters
    staticAppAssignment = par("staticAppAssignment");

    // Get the number of input and output gates
    lower.inBaseId = gateBaseId("fromInput");
    lower.outBaseId = gateBaseId("toInput");
    upper.inBaseId = gateBaseId("fromOutput");
    upper.outBaseId = gateBaseId("toOutput");

    upperSize = gateSize("toOutput");
    lowerSize = gateSize("toInput");
}

void CheckingHubModule::finish()
{
    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *CheckingHubModule::getOutGate(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    int arrivalId = arrivalGate->getBaseId();

    if (arrivalId == lower.inBaseId)
    {
        return gate(lower.outBaseId + arrivalGate->getIndex());
    }
    else if (arrivalId == upper.inBaseId)
    {
        return gate(upper.outBaseId + arrivalGate->getIndex());
    }
    else
        error("Message received from an unknown gate [%s]", msg->getName());
        
    return nullptr;
}

void CheckingHubModule::processSelfMessage(cMessage *msg)
{
    error("This module cannot process self messages:%s", msg->getName());
}

void CheckingHubModule::processRequestMessage(SIMCAN_Message *sm)
{
    cGate *arrivalGate = sm->getArrivalGate();
    int arrivalId = arrivalGate->getBaseId();

    if (arrivalId == lower.inBaseId)
    {
        // lower -> upper
        int idx = (upperSize == 1) ? 0 : sm->getNextModuleIndex();
        sendRequestMessage(sm, upper.outBaseId + idx);
    }
    else
    {
        // upper -> lower
        int idx = (lowerSize == 1) ? 0 : sm->getNextModuleIndex();
        sendRequestMessage(sm, lower.outBaseId + idx);
    }
}

void CheckingHubModule::processResponseMessage(SIMCAN_Message *sm)
{

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    sendResponseMessage(sm);
}

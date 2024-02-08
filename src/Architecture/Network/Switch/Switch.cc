#include "Switch.h"

Define_Module(Switch);

void Switch::initialize()
{

    int numGates, i;

    // Init the super-class
    cSIMCAN_Core::initialize();

    // Get the number of input and output gates
    numGates = gateSize("in");

    // Init the cGates vector for inputs
    inputGates = new cGate *[numGates];
    outputGates = new cGate *[numGates];

    for (i = 0; i < numGates; i++)
    {
        inputGates[i] = gate("in", i);
        outputGates[i] = gate("out", i);

        if (!(outputGates[i]->isConnected()))
        {
            error("Gate is not connected");
        }
    }

    // Get switch type
    const char *typeChr = par("type");
    type = typeChr;
}

void Switch::finish()
{

    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *Switch::getOutGate(cMessage *msg)
{

    cGate *outGate;

    // Init...
    outGate = nullptr;

    // If msg arrives
    if (msg->arrivedOn("in"))
    {
        outGate = gate("out", msg->getArrivalGate()->getIndex());
    }

    // Msg arrives from an unknown gate
    else
        error("Message received from an unknown gate [%s]", msg->getName());

    return outGate;
}

void Switch::processSelfMessage(cMessage *msg)
{
    error("This module cannot process self messages:%s", msg->getName());
}

void Switch::processRequestMessage(SIMCAN_Message *sm)
{

    // TODO: Finish switch behaviour!

    // Switch is allocated in a rack
    if (type.compare("rack") == 0)
    {
    }

    // Switch is allecated in a Board
    else if (type.compare("board") == 0)
    {
    }

    // Unknown switch type
    else
        error("Unknown Switch type: %s", type.c_str());
}

void Switch::processResponseMessage(SIMCAN_Message *sm)
{

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    sendResponseMessage(sm);
}

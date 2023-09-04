#include "IotSensor.h"

Define_Module(IotSensor);

void IotSensor::initialize()
{
    // Create the "ping" message
    ping = new SIMCAN_Message();
    ping->setOperation(SM_Edge_Ping);
    ping->setIsResponse(false);

    // Retrieve the update parameter
    updatePeriod = par("updatePeriod");
    scheduleAt(0.0,ping);

    cSIMCAN_Core::initialize();

    EV << "Iot sensor ready" << endl;
}

void IotSensor::finish()
{
    cancelAndDelete(ping);
}

void IotSensor::processSelfMessage(cMessage *msg)
{
    // This is pretty much a infinite loop
    sendRequestMessage(ping->dup(), gate("out"));
    scheduleAt(simTime() + updatePeriod, ping);
}

cGate *IotSensor::getOutGate(cMessage *msg) { return gate("out"); }
void IotSensor::processRequestMessage(SIMCAN_Message *sm) { error("This module doesn't take requests (yet)"); }

void IotSensor::processResponseMessage(SIMCAN_Message *sm)
{
    EV << "ACK recieved" << endl;
    delete sm;
}

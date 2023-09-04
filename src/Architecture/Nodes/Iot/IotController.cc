#include "IotController.h"

Define_Module(IotController);

void IotController::initialize()
{
    // Init steps
    updateCounter = 0;
    fogRequest = createTestFogIORequest();

    cSIMCAN_Core::initialize();
}

void IotController::finish()
{
    if (fogRequest != nullptr)
        delete fogRequest;
}

cGate *IotController::getOutGate(cMessage *msg)
{
    // It's the case for sending the request to the fog
    if (dynamic_cast<SM_FogIO*>(msg) != nullptr)
        return gate("toFog");

    return gate("toIot", msg->getArrivalGate()->getIndex());
}

void IotController::processSelfMessage(cMessage *msg) { error("Unkown self message"); }

void IotController::processRequestMessage(SIMCAN_Message *msg)
{
    if (msg->getOperation() == SM_Edge_Ping)
    {
        updateCounter++;

        // Send an "ACK"
        msg->setIsResponse(true);
        sendResponseMessage(msg);

        if (updateCounter > 10)
        {
            EV << "Sending I/O operation to the Fog" << endl;
            updateCounter = 0;
            sendRequestMessage(fogRequest, gate("toFog"));
            fogRequest = nullptr;
        }
    }
    else
    {
        error("Unkown operation type");
    }
}

void IotController::processResponseMessage(SIMCAN_Message *msg)
{
    if (msg->getOperation() == SM_Fog_Read || msg->getOperation() == SM_Fog_Write)
    {
        // Remember that the so called response is the very same request we sent
        SM_FogIO *response = dynamic_cast<SM_FogIO *>(msg);
        EV << "The I/O operation (" << response->getRequestSize() << ") exited with code: " << response->getResult() << endl;
        EV << "Resetting existing request... " << endl;

        // Recovering and resetting the message
        response->setIsResponse(false);
        fogRequest = response;
    }
    else
    {
        error("Unkown response");
    }
}

/* TEMPORARY SECTION */
SM_FogIO *IotController::createTestFogIORequest()
{
    SM_FogIO *request = new SM_FogIO();

    request->setIsResponse(false);
    request->setOperation(SM_Fog_Write);
    request->setRequestSize(50);

    return request;
}

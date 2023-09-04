#include "FogNode.h"

Define_Module(FogNode);

#define SEND_REQUEST_DC 1
#define SEND_RESPONSE 2

void FogNode::initialize()
{
    // Init the super class
    cSIMCAN_Core::initialize();

    // For the time being it's does everything in the first stage
    EV << "Hello there!" << endl;

    // Init the counter and the auto self message
    numIORequests = 0;
    event = new cMessage("event");
    vmsRequest = nullptr;
    appsRequest = nullptr;

    cSIMCAN_Core::initialize();

    EV << "Start ready" << endl;
}

void FogNode::finish()
{
    cSIMCAN_Core::finish();

    if (vmsRequest != nullptr)
        delete vmsRequest;
    if (appsRequest != nullptr)
        delete appsRequest;
    cancelAndDelete(event);
}

void FogNode::processSelfMessage(cMessage *msg)
{
    switch (msg->getKind())
    {
    case SEND_RESPONSE:
        sendResponseMessage(dynamic_cast<SIMCAN_Message *>(msg));
        break;

    case SEND_REQUEST_DC:{
        SIMCAN_Message *m = dynamic_cast<SIMCAN_Message *>(msg->getControlInfo());
        sendRequestMessage(m, gate("toDataCentre"));
        break;
    }
    default:
        // If nothing is matched
        error("Error in self message (event) : Message kind unkown");
    }
}

cGate *FogNode::getOutGate(cMessage *msg)
{
    // If it is a request incoming, select the out gate which points to the requester
    if (dynamic_cast<SM_FogIO *>(msg) != nullptr)
        return gate("toEdge", msg->getArrivalGate()->getIndex());

    // In other case (temporally) select the DataCentre gate
    return gate("toDataCentre");
}

void FogNode::processRequestMessage(SIMCAN_Message *sm)
{
    if (sm->getOperation() == SM_Fog_Read || sm->getOperation() == SM_Fog_Write)
    {
        // Check that it is an IO request
        SM_FogIO *request = dynamic_cast<SM_FogIO *>(sm);
        if (request == nullptr)
            error("Wrong message type for an IO Fog operation");

        // Increment the counter
        numIORequests++;

        // Retrieve the storage bandwidth
        double storageBandwidth = par("storageBandwidth");

        // The unit's check : MB / (Mbit/s * 1 byte/ 8 bit)
        double delay = request->getRequestSize() / (storageBandwidth / 8);

        // Set response flag, the result and the kind for the scheduled event
        request->setIsResponse(true);
        request->setResult(0);
        request->setKind(SEND_RESPONSE);

        // FIXME If it's still modeled like it is, the class SM_FogIO
        // should change it's size for the modeling of the transmission
        // on the datarate channel depending on the operation type (R/W)
        // when it is indicated that the response flag is activated

        // Prepare to send response by scheduling it
        scheduleAt(simTime() + delay, sm);

        if (numIORequests > 10)
        {
            // Reset and launch an request to the datacentre
            numIORequests = 0;
            vmsRequest = createVmTestRequest();
            event->setKind(SEND_REQUEST_DC);
            EV << "Request for the DC would be scheduled" << endl;
            //scheduleAt(simTime() + 1.0, event);
            //vmsRequest = nullptr;
        }
    }
    else
    {
        error("Unkown operation");
    }
}

void FogNode::processResponseMessage(SIMCAN_Message *sm)
{
    // FIXME Handle or notify the fact that an app request was completed or not !
    // Print the response
    SM_UserVM *response = dynamic_cast<SM_UserVM *>(sm);
    if (response != nullptr)
    {
        if (response->getResult() == SM_VM_Res_Reject)
            EV << "Request denied" << endl;
        else
        {
            VM_Response *vm = response->getResponse(0, 0);
            response->printUserVM();

            EV << "Request for the vm was succesfull" << vm->strIp << endl;
            appsRequest = createAppTestRequest(response);

            // Prepare both the action and the message
            event->setKind(SEND_REQUEST_DC);
            event->setControlInfo(appsRequest);

            scheduleAt(simTime() + 1.0, event);

            // Regen the vm's request for further use
            response->setIsResponse(false);
            vmsRequest = response;
        }
    }
    else
    {
        EV << "Message of kind" << sm->getClassName() << " recieved" << endl;
    }
}

/* TEMPORARY SECTION */
SM_UserVM *FogNode::createVmTestRequest()
{
    SM_UserVM *request = new SM_UserVM();

    request->setUserID("fog-node-request");
    request->createNewVmRequest("VM_small", "1", 1000, 50, 10, 10);

    request->setIsResponse(false);
    request->setOperation(SM_VM_Req);

    return request;
}

SM_UserAPP *FogNode::createAppTestRequest(SM_UserVM *vm_request)
{
    SM_UserAPP *request = new SM_UserAPP();

    // Select the given VM -- In this case we only allocated one
    VM_Response *vm1 = vm_request->getResponse(0, 0);

    request->setUserID("fog-node-request");
    request->createNewAppRequest(
        "test 1", "AppDataIntensive", vm1->strIp, // The instance name, the type and the place where to be executed
        "1", vm1->startTime                       // The vm ID and it's start time
    );

    request->setIsResponse(false);
    request->setOperation(SM_APP_Req);

    return request;
}

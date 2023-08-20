#include "FogNode.h"

Define_Module(FogNode);

#define SEND_VM_REQ 1
#define SEND_APP_REQ 2

void FogNode::initialize() {
    // Init the super class
    cSIMCAN_Core::initialize();

    // For the time being it's does everything in the first stage
    EV << "Hello there!" << endl;
    outGate = gate("out");

    // Create an example Vm Request
    vmsRequest = createVmTestRequest();

    event = new cMessage("SELF", SEND_VM_REQ);
    scheduleAt(0.0, event);
    EV << "Start ready" << endl;
}

void FogNode::finish() {
    cSIMCAN_Core::finish();
    delete vmsRequest;
    delete event;
}

void FogNode::processSelfMessage(cMessage *msg) {
    // Create and send the request to the CloudManager
    // It should be good enough to send an app request
    if (msg->getKind() == SEND_VM_REQ) {
        sendRequestMessage(vmsRequest, outGate);
    } else if (msg->getKind() == SEND_APP_REQ) {
        // Send the app request
    } else {
        error("Message kind unkown");
    }
}

void FogNode::processRequestMessage(SIMCAN_Message *sm) {
    // Currently we don't take requests !
    error("This module doesn't take requests (yet)");
}

void FogNode::processResponseMessage(SIMCAN_Message *sm) {
    // Print the response
    SM_UserVM *response = dynamic_cast<SM_UserVM*>(sm);
    if (response != nullptr) {
        if (response->getResult() == SM_VM_Res_Reject)
            EV << "Request denied" << endl;
        else {
            VM_Response *vm = response->getResponse(0, 0);
            response->printUserVM();

            EV << "Request for the vm was succesfull" << vm->strIp << endl;
        }
    } else {
        EV << "Message of kind" << sm->getClassName() << "recieved" << endl;
    }
}

cGate* FogNode::getOutGate(cMessage *msg) {
    // Select the out gate of the node
    // For the time being, and for simplicity, there's only one gate to speak with the datacenter
    return outGate;
}

/* TEMPORARY SECTION */
SM_UserVM* FogNode::createVmTestRequest() {
    SM_UserVM *request = new SM_UserVM();

    request->setUserID("fog-node-request");
    request->createNewVmRequest("VM_small", "1", 1000, 50, 10, 10);
    request->setIsResponse(false);
    request->setOperation(SM_VM_Req);

    return request;
}

SM_UserAPP* FogNode::createAppTestRequest() {
    return nullptr;
}

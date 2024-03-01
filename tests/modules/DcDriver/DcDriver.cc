#include "DcDriver.h"
using namespace inet;
Define_Module(DcDriver);

void DcDriver::initialize()
{
    // Parse target's address
    if (!dcAddress.tryParse(par("dcAddress")))
        error("Unable to parse the DC Address, ¿wrong input?");

    // Prepare request
    request = new SM_UserVM();
    request->setUserId("TestUser");
    request->setOperation(SM_VM_Req);
    VM_Request::InstanceRequestTimes times = {
        .maxStartTime = 1000,       // Placeholder, only relevant on user generator
        .rentTime = 5,              // In seconds, 3600s == 1h of vm renting
        .maxSubTime = 1000,         // Placeholder, only relevant on cloud provider (set on user generator)
        .maxSubscriptionTime = 1000 // God knows what this does
    };

    request->createNewVmRequest("VM_small", "vm1", times);
    request->addNewIp(dcAddress);

    scheduleAt(0.0, new cMessage("StartTest"));
    cSIMCAN_Core::initialize();
}

void DcDriver::finish()
{
    delete request;
}

cGate *DcDriver::getOutGate(cMessage *sm)
{
    return gate("out");
}

void DcDriver::processSelfMessage(cMessage *msg)
{
    EV << "Sending requests to Dc\n";
    sendRequestMessage(request, gate("out"));
    delete msg;
}

void DcDriver::processResponseMessage(SIMCAN_Message *sm)
{
    EV << "Recieved response, vm accepted?: " << (sm->getResult() == SM_VM_Res_Accept) << '\n';
    auto vmRequest = check_and_cast<SM_UserVM *>(sm);

    for (int i = 0; i < vmRequest->getVmArraySize(); i++)
    {
        VM_Response *response;
        if (vmRequest->getResponse(i, &response))
            EV << "Got response for: " << vmRequest->getVm(i).vmId << " assigned service-url: " << response->ip << '\n';
        else
            EV << "Got no response for: " << vmRequest->getVm(i).vmId << '\n';
    }
}

void DcDriver::processRequestMessage(SIMCAN_Message *sm)
{
    EV << "Recieved request, this should be the rescuing op\n";
}
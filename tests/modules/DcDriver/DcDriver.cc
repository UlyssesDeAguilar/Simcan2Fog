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
    VM_Request::InstanceRequestTimes times = {
        .maxStartTime = 1000,        // Placeholder, only relevant on user generator
        .maxSubTime = 1000,          // Placeholder, only relevant on cloud provider (set on user generator)
        .maxSubscriptionTime = 1000, // God knows what this does
        .rentTime = 3600             // In seconds, 3600s == 1h of vm renting
    };

    request->createNewVmRequest("VM_small", "vm1", times);
    request->addNewIp(dcAddress);

    scheduleAt(0.0, new cMessage("StartTest"));
}

void DcDriver::finish()
{
    delete request;
}

void DcDriver::handleMessage(cMessage *msg)
{
    // Start message
    if (msg->isSelfMessage())
    {
        EV << "Sending requests to Dc\n";
        send(request, "out");
        return;
    }

    // Cast
    auto sm = check_and_cast<SIMCAN_Message *>(msg);

    if (sm->isResponse())
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
    else
    {
        EV << "Recieved request, this should be the rescuing op\n";
        // TODO
    }
}
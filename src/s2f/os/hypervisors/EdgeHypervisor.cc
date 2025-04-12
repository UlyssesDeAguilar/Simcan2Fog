#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    if (stage == LOCAL)
    {
        Hypervisor::initialize();
        appsVector = getParentModule()->getSubmodule("apps");
        serialOut = gate("serialOut");
    }
    else if (stage == NEAR)
    {
        int vmId = controlTable->allocateVm();
        controlTable->createMapping(par("hostName"), vmId);
    }
}

void EdgeHypervisor::sendEvent(cMessage *msg) { send(msg, serialOut); }

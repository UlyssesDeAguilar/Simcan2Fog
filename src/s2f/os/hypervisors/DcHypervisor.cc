#include "DcHypervisor.h"

using namespace hypervisor;

Define_Module(DcHypervisor);

void DcHypervisor::initialize(int stage)
{
    if (stage == LOCAL)
    {
        // Let the parent setup initialize first
        Hypervisor::initialize();

        // Init module parameters
        // nPowerOnTime = par("powerOnTime");
        powerMessage = nullptr;
        netDb = check_and_cast<SimpleNetDb *>(findModuleByPath(par("netDbPath")));

        cModule *schedulerVector = getParentModule()->getSubmodule("cpuSchedVector");
        int vectorSize = schedulerVector->getSubmodule("cpuScheduler", 0)->getVectorSize();
        schedulers.resize(vectorSize);

        for (int i = 0; i < vectorSize; i++)
            schedulers[i] = check_and_cast<CpuSchedulerRR *>(schedulerVector->getSubmodule("cpuScheduler", i));
    }
    else if (stage == NEAR)
    {
        netDb->registerHost(par("hostName"), hardwareManager->getIp());
    }
}

void DcHypervisor::finish()
{
    // Finish the super-class
    schedulers.clear();
    Hypervisor::finish();
}

void DcHypervisor::sendEvent(cMessage *msg)
{
    int destination = netDb->getAddress("manager");
    sendToNetwork(msg, destination);
}

/**void DcHypervisor::processSelfMessage(cMessage *msg)
{
    if (!strcmp(msg->getName(), "POWERON_MACHINE"))
    {
        setActive(true);
        powerMessage = nullptr;
    }

    delete msg;
}*/

void DcHypervisor::powerOn(bool active)
{
    Enter_Method_Silent();
    setActive(active);
    /*if (active)
    {
        powerMessage = new cMessage("POWERON_MACHINE");
        scheduleAt(simTime() + SimTime(nPowerOnTime, SIMTIME_S), powerMessage);
    }
    else
    {
        setActive(active);
    }*/
}

void DcHypervisor::handleVmTimeout(VmControlBlock &vm)
{
    // Set vm as suspended, this will force to hold requests/responses for the vm
    vm.state = vmSuspended;

    // Create extension offer
    auto extensionOffer = new SM_VmExtend();
    extensionOffer->setVmId(controlTable->getGlobalVmId(vm.vmId));
    extensionOffer->setUserId(vm.userId.c_str());
    extensionOffer->setIsResponse(false);

    // FIXME: Send to the manager
    send(extensionOffer, networkGates.outBaseId);
}
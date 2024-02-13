#include "DcHypervisor.h"
using namespace hypervisor;

Define_Module(DcHypervisor);

void DcHypervisor::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init module parameters
    nPowerOnTime = par("powerOnTime");
    powerMessage = nullptr;

    // Get base Id for indexing
    appGates.inBaseId = gateBaseId("fromApps");
    appGates.outBaseId = gateBaseId("toApps");

    // Get base Id for indexing
    schedulerGates.inBaseId = gateBaseId("fromCpuScheduler");
    schedulerGates.outBaseId = gateBaseId("toCpuScheduler");

    // Load both apps vector and scheduler vector
    cModule *osModule = getParentModule();

    auto schedulerAccessor = [](cModule *osModule, int i) -> cModule *
    { return osModule->getSubmodule("cpuSchedVector")->getSubmodule("cpuScheduler", 0); };

    loadVector(schedulers, osModule, schedulerAccessor);
}

void DcHypervisor::loadVector(std::vector<cModule *> &v, cModule *osModule, cModule *(*accessor)(cModule *, int))
{
    // Get the basic info about the vector
    cModule *base = accessor(osModule, 0);
    int vectorSize = base->getVectorSize();

    // Reshape vector to needed space
    v.resize(vectorSize);
    v.shrink_to_fit();

    // Start loading
    for (int i = 0; i < vectorSize; i++)
        v[i] = accessor(osModule, i);
}

void DcHypervisor::finish()
{
    // Finish the super-class
    cSIMCAN_Core::finish();
}

void DcHypervisor::processSelfMessage(cMessage *msg)
{
    if (!strcmp(msg->getName(), "POWERON_MACHINE"))
    {
        setActive(true);
        powerMessage = nullptr;
    }

    delete msg;
}

void DcHypervisor::powerOn(bool active)
{
    Enter_Method_Silent();
    if (active)
    {
        powerMessage = new cMessage("POWERON_MACHINE");
        scheduleAt(simTime() + SimTime(nPowerOnTime, SIMTIME_S), powerMessage);
    }
    else
    {
        setActive(active);
    }
}

void DcHypervisor::processResponseMessage(SIMCAN_Message *sm)
{

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    // Send back the message
    sendResponseMessage(sm);
}

cModule *DcHypervisor::handleVmRequest(const VM_Request &request)
{
    unsigned int *cpuCoreIndex;

    // Query vm type
    const VirtualMachine *vm = dataManager->searchVirtualMachine(request.vmType);

    // Extract requested resources
    uint32_t cores = vm->getNumCores();
    double memory = vm->getMemoryGb();
    double disk = vm->getDiskGb();

    // Attempt allocating needed space
    if (!hardwareManager->tryAllocateResources(cores, memory, disk, &cpuCoreIndex))
        return nullptr;

    // Got it, get the vm an id and register it
    uint32_t id = vmsControl.takeId();
    vmIdMap[request.vmId] = id;

    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR *>(schedulers[id]);

    bool *isCPU_Idle = new bool[cores];
    for (int i = 0; i < cores; i++)
        isCPU_Idle[i] = true;

    pVmScheduler->setManagedCpuCores(cores);
    pVmScheduler->setCpuCoreIndex(cpuCoreIndex);
    pVmScheduler->setIsCpuIdle(isCPU_Idle);
    pVmScheduler->setIsRunning(true);

    return getParentModule()->getSubmodule("appsVectors", id);
}

void DcHypervisor::deallocateVmResources(const std::string &vmId)
{
    // This could be fixed with an approach similar to AppControlBlock
    auto id = getOrDefault(vmIdMap, vmId, UINT32_MAX);

    // If not found
    if (id == UINT32_MAX)
        return;

    // Recover scheduler and original request
    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR *>(schedulers[id]);
    auto resourceRequest = vmsControl[id].request;

    // Extract requested resources
    uint32_t cores = resourceRequest->getTotalCpus();
    double memory = resourceRequest->getTotalMemory();
    double disk = resourceRequest->getTotalDiskGb();
    auto cpuCoreIndex = pVmScheduler->getCpuCoreIndex();

    // Free the resources
    hardwareManager->deallocateResources(cores, memory, disk, cpuCoreIndex);

    // Shutdown scheduler
    pVmScheduler->setIsRunning(false);

    // Release VM
    vmsControl.releaseId(id);
}
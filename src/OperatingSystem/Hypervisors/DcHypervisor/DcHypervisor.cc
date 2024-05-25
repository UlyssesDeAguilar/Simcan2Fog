#include "DcHypervisor.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"

using namespace hypervisor;

Define_Module(DcHypervisor);

void DcHypervisor::initialize(int stage)
{
    // Let the parent setup initialize first
    Hypervisor::initialize(stage);

    switch (stage)
    {
    case LOCAL:
    {
        // Init module parameters
        nPowerOnTime = par("powerOnTime");
        powerMessage = nullptr;

        // Load both apps vector and scheduler vector
        cModule *osModule = getParentModule();

        auto schedulerAccessor = [](cModule *osModule, int i) -> cModule *
        { return osModule->getSubmodule("cpuSchedVector")->getSubmodule("cpuScheduler", 0); };

        loadVector(schedulers, osModule, schedulerAccessor);

        diskManager = check_and_cast<DiskManager *>(osModule->getParentModule()->getSubmodule("disk"));
        break;
    }
    case BLADE:
    {
        // This is the reason why I would like to do this with messages instead
        resourceManager = check_and_cast<DcResourceManager *>(getModuleByPath("^.^.^.resourceManager"));
        localUrl.setLocalAddress(hardwareManager->getIp());
        resourceManager->registerNode(hardwareManager->getIp(), hardwareManager->getAvailableResources());
        break;
    }
    default:
        break;
    }
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

/*void DcHypervisor::processResponseMessage(SIMCAN_Message *sm)
{

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    // Send back the message
    sendResponseMessage(sm);
}*/

cModule *DcHypervisor::handleVmRequest(const VM_Request &request, const char *userId)
{
    Enter_Method_Silent("DcHypervisor, handling vm request...");

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
    VmControlBlock &controlBlock = vmsControl[id];

    // Register the global id in map and in the control block
    vmIdMap[request.vmId] = id;
    auto iter = vmIdMap.find(request.vmId);
    controlBlock.globalId = &iter->first;
    controlBlock.userId = userId;
    controlBlock.vmType = vm;
    controlBlock.state = vmRunning;

    // Schedule timeout message -- if it is the first time it's scheduled
    if (!controlBlock.timeOut)
    {
        controlBlock.timeOut = new cMessage("VM timeout", AutoEvent::VM_TIMEOUT);
        controlBlock.timeOut->setContextPointer(&controlBlock);
    }
    scheduleAt(simTime() + request.times.rentTime, controlBlock.timeOut);

    // Setup the scheduler
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

void DcHypervisor::handleVmTimeout(VmControlBlock &vm)
{
    // Set vm as suspended, this will force to hold requests/responses for the vm
    vm.state = vmSuspended;

    // Create extension offer
    auto extensionOffer = new SM_VmExtend();

    // Fill in the neccesary details
    extensionOffer->setVmId(vm.globalId->c_str());
    extensionOffer->setUserId(vm.userId.c_str());

    // Set as a request and send to the Manager
    extensionOffer->setIsResponse(false);
    extensionOffer->setOperation(SM_ExtensionOffer);

    // Send to the manager
    auto routingInfo = new RoutingInfo();
    routingInfo->setDestinationUrl(ServiceURL(DC_MANAGER_LOCAL_ADDR));
    routingInfo->setSourceUrl(localUrl);
    extensionOffer->setControlInfo(routingInfo);
    sendRequestMessage(extensionOffer, networkGates.outBaseId);
}

void DcHypervisor::extendVm(const std::string &vmId, int extensionTime)
{
    Enter_Method_Silent();

    // This could be fixed with an approach similar to AppControlBlock
    auto id = getOrDefault(vmIdMap, vmId, UINT32_MAX);

    // If not found
    if (id == UINT32_MAX)
    {
        error("Extending vm for an unkown global id, check ServiceURL");
        return;
    }

    VmControlBlock &block = vmsControl.at(id);
    block.state = vmRunning;

    // Dispatch all holded requests/responses
    for (const auto &request : block.callBuffer)
    {
        if (request->isResponse())
            sendResponseMessage(request);
        else
            osCore.processSyscall(request);
    }

    // Flush the buffer
    block.callBuffer.clear();

    // Reschedule the timer
    scheduleAt(simTime() + extensionTime, block.timeOut);
}

void DcHypervisor::deallocateVmResources(const std::string &vmId)
{
    Enter_Method_Silent();

    // This could be fixed with an approach similar to AppControlBlock
    auto id = getOrDefault(vmIdMap, vmId, UINT32_MAX);

    // If not found
    if (id == UINT32_MAX)
    {
        error("Deallocating vm for an unkown global id, check ServiceURL");
        return;
    }

    // Delete the timer
    VmControlBlock &block = vmsControl.at(id);
    cancelAndDelete(block.timeOut);

    // Force app termination
    for (auto &app : block.apps)
    {
        // If the app is active and running, terminate it
        if (app.first == true && app.second.isRunning())
            osCore.handleAppTermination(app.second, true);
    }

    // Flush buffer with requests/responses
    for (const auto &request : block.callBuffer)
        delete request;
    block.callBuffer.clear();

    // Recover scheduler and original request
    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR *>(schedulers[id]);
    const VirtualMachine *vm = block.vmType;

    // Extract requested resources
    uint32_t cores = vm->getNumCores();
    double memory = vm->getMemoryGb();
    double disk = vm->getDiskGb();
    auto cpuCoreIndex = pVmScheduler->getCpuCoreIndex();

    // Shutdown scheduler
    pVmScheduler->setIsRunning(false);
    diskManager->stopVmQueue(block.vmId);

    // Free the resources
    hardwareManager->deallocateResources(cores, memory, disk, cpuCoreIndex);

    // Release VM
    vmsControl.releaseId(id);

    // Notify the resource manager
    bool idleNode = hardwareManager->getTotalResources().vms == hardwareManager->getAvailableResources().vms;
    resourceManager->confirmNodeDeallocation(hardwareManager->getIp(), vm, idleNode);
}
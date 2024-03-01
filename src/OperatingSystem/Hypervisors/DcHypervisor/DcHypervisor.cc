#include "DcHypervisor.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"

using namespace hypervisor;

Define_Module(DcHypervisor);

void DcHypervisor::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
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
        break;
    }
    case BLADE:
    {
        // This is the reason why I would like to do this with messages instead
        resourceManager = check_and_cast<DcResourceManager *>(getModuleByPath("^.^.^.resourceManager"));
        resourceManager->registerNode(hardwareManager->getIp(), hardwareManager->getAvailableResources());
        break;
    }
    default:
        break;
    }

    Hypervisor::initialize(stage);
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

// This is the older code for scheduling the timeout, it implies that sucribe operations need special treatment -- Look at user generator!
// double rentTime;
//             // Getting VM and scheduling renting timeout
//             strVmId = userVM_Rq->getVmId();
//             if (!strVmId.empty() && userVM_Rq->getOperation() == SM_VM_Sub)
//                 rentTime = 3600;
//             else
//                 rentTime = vmRequest.times.rentTime.dbl();
//
//             // vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.strVmType, nRentTime);
//             vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, userId, vmRequest, rentTime);

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
    auto controlBlock = vmsControl[id];

    // Register the global id in map and in the control block
    vmIdMap[request.vmId] = id;
    auto iter = vmIdMap.find(request.vmId);
    controlBlock.globalId = &iter->first;
    controlBlock.userId = userId;

    // Schedule timeout message
    cMessage *timeOut = new cMessage("VM timeout", AutoEvent::VM_TIMEOUT);
    timeOut->setContextPointer(&controlBlock);
    scheduleAt(simTime() + request.times.rentTime, timeOut);

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
    // Create extension offer
    auto extensionOffer = new SM_VmExtend();

    // Fill in the neccesary details
    extensionOffer->setVmId(vm.globalId->c_str());
    extensionOffer->setUserId(vm.userId.c_str());

    // Set as a request and send to the Manager
    extensionOffer->setIsResponse(false);
    extensionOffer->setOperation(SM_APP_Req);
    extensionOffer->setOperation(SM_APP_Res_Timeout);

    auto localUrl = ServiceURL(DC_MANAGER_LOCAL_ADDR);
    auto routingInfo = new RoutingInfo();
    routingInfo->setUrl(localUrl);

    extensionOffer->setControlInfo(routingInfo);

    /*
    This would be after the definitive response of the User of abandoning or recovering the vm!

    // Deallocate the vm resources
    deallocateVmResources(*vm.globalId);

    // For all app entries
    for (auto &app : vm.apps)
    {
        // If the app is active and running, terminate it
        if (app.first == true && app.second.isRunning())
            osCore.handleAppTermination(app.second, true);
    }
    */
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
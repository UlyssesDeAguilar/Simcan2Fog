#include "Hypervisor.h"

Define_Module(Hypervisor);

Hypervisor::~Hypervisor()
{
    mapVmScheduler.clear();
    mapResourceRequestPerVm.clear();
}

void Hypervisor::initialize()
{

    int i, numCpuGates, numAppGates;

    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init module parameters
    isVirtualHardware = par("isVirtualHardware");
    maxVMs = (unsigned int)par("maxVMs");
    nPowerOnTime = par("powerOnTime");
    powerMessage = nullptr;
    //            numAllocatedVms = 0;

    // Get the number of gates for each vector
    numAppGates = gateSize("fromApps");
    numCpuGates = gateSize("fromCpuScheduler");

    // Init the size of the cGate vectors
    fromAppsGates = new cGate *[numAppGates];
    toAppsGates = new cGate *[numAppGates];

    // Init the cGates vector for Applications
    for (i = 0; i < numAppGates; i++)
    {
        fromAppsGates[i] = gate("fromApps", i);
        toAppsGates[i] = gate("toApps", i);

        // Checking connections
        if (!toAppsGates[i]->isConnected())
        {
            EV_ERROR << "toAppsGates[" << i << "] is not connected";
            error("toAppsGates is not connected");
        }
    }

    // Init the cGates vector for CPU scheduler
    fromCPUGates = new cGate *[numCpuGates];
    toCPUGates = new cGate *[numCpuGates];

    for (i = 0; i < numCpuGates; i++)
    {
        fromCPUGates[i] = gate("fromCpuScheduler", i);
        toCPUGates[i] = gate("toCpuScheduler", i);
    }

    pAppsVectors = getParentModule()->getParentModule()->getSubmodule("appsVectors", 0);
    pAppsVectorsArray = new cModule *[pAppsVectors->getVectorSize()];

    for (int i = 0; i < pAppsVectors->getVectorSize(); i++)
        pAppsVectorsArray[i] = getParentModule()->getParentModule()->getSubmodule("appsVectors", i);

    pCpuScheds = getModuleByPath("^.cpuSchedVector")->getSubmodule("cpuScheduler", 0);
    pCpuSchedArray = new cModule *[pCpuScheds->getVectorSize()];
    freeSchedArray = new bool[pCpuScheds->getVectorSize()];
    for (int i = 0; i < pCpuScheds->getVectorSize(); i++)
    {
        pCpuSchedArray[i] = getModuleByPath("^.cpuSchedVector")->getSubmodule("cpuScheduler", i);
        freeSchedArray[i] = true;
    }

    hardwareManager = check_and_cast<HardwareManager *>(getModuleByPath("^.^.hardwareManager"));
}

void Hypervisor::finish()
{

    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *Hypervisor::getOutGate(cMessage *msg)
{
    // If msg arrives from Application gates
    if (msg->arrivedOn("fromApps"))
    {
        return gate("toApps", msg->getArrivalGate()->getIndex());
    }
    // If msg arrives from CPU scheduler
    else if (msg->arrivedOn("fromCpuScheduler"))
    {
        error("This module cannot receive requests from the CPU system!");
    }
    // Msg arrives from an unknown gate
    else
        error("Message received from an unknown gate [%s]", msg->getName());

    return nullptr;
}

void Hypervisor::processSelfMessage(cMessage *msg)
{
    if (!strcmp(msg->getName(), "POWERON_MACHINE"))
    {
        setActive(true);
        powerMessage = nullptr;
    }

    delete msg;
}

void Hypervisor::processRequestMessage(SIMCAN_Message *sm)
{

    // Message came from CPU
    if (sm->arrivedOn("fromCpuScheduler"))
    {
        error("This module cannot receive request messages from CPU!!!");
        return;
    }

    if (!sm->arrivedOn("fromApps"))
    {
        // Unknown operation!
        error("Unkown sender");
        return;
    }

    // Message came from applications
    EV_DEBUG << "(processRequestMessage) Message arrives from applications." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    // CPU operation?
    if (!(sm->getOperation() == SM_ExecCpu || sm->getOperation() == SM_AbortCpu))
    {
        error("Unknown operation:%d", sm->getOperation());
        return;
    }

    // Virtual HW! There is only 1 CPU scheduler
    if (!isVirtualHardware)
    {

        // Debug
        EV_INFO << "Sending request message to CPU." << endl
                << sm->contentsToString(showMessageContents, showMessageTrace).c_str() << endl;

        sendRequestMessage(sm, toCPUGates[0]);
    }
    else
    {
        // TODO: Manage users/VMs to redirec requests to the corresponding CPU scheduler

        // Debug
        EV_INFO << "Sending request message to CPU." << endl
                << sm->contentsToString(showMessageContents, showMessageTrace).c_str() << endl;

        // Checks for the next module
        if (sm->getNextModuleIndex() == SM_UnsetValue)
        {
            error("Unset value for nextModuleIndex... and there are several output gates. %s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
        }

        // Next module index is out of bounds...
        else if ((sm->getNextModuleIndex() < 0) || (sm->getNextModuleIndex() >= gateSize("toCpuScheduler")))
        {
            error("nextModuleIndex %d is out of bounds [%d]", sm->getNextModuleIndex(), gateSize("toCpuScheduler"));
        }

        // Everything is OK... send the message! ;)
        else
        {

            // Debug (Debug)
            EV_DEBUG << "(processRequestMessage) Sending request message. There are several output gates -> toOutputGates[" << sm->getNextModuleIndex() << "]" << endl
                     << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

            sendRequestMessage(sm, toCPUGates[sm->getNextModuleIndex()]);
        }
    }
}

bool Hypervisor::isInUse()
{
    for (int i = 0; i < maxVMs; i++)
    {
        if (!freeSchedArray[i])
        {
            return true;
        }
    }
    return false;
}


void Hypervisor::powerOn(bool active)
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

void Hypervisor::processResponseMessage(SIMCAN_Message *sm)
{

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message." << endl
             << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    // Send back the message
    sendResponseMessage(sm);
}

cModule *Hypervisor::allocateNewResources(NodeResourceRequest *pResourceRequest)
{
    unsigned int *cpuCoreIndex;

    // Extract requested resources
    uint32_t cores = pResourceRequest->getTotalCpus();
    double memory = pResourceRequest->getTotalMemory();
    double disk = pResourceRequest->getTotalDiskGb();

    if (!hardwareManager->tryAllocateResources(cores, memory, disk, &cpuCoreIndex))
        return nullptr;

    bool allocatedVm = false;
    int nSchedulerIndex = -1;

    for (int i = 0; i < maxVMs && !allocatedVm; i++)
    {
        if (freeSchedArray[i])
        {
            nSchedulerIndex = i;
            freeSchedArray[nSchedulerIndex] = false;
            allocatedVm = true;
        }
    }

    if (!allocatedVm)
        return nullptr;

    cModule *pVmSchedulerModule = pCpuSchedArray[nSchedulerIndex];
    cModule *pVmAppVectorModule = pAppsVectorsArray[nSchedulerIndex];

    mapResourceRequestPerVm[pResourceRequest->getVmId()] = pResourceRequest;
    mapVmScheduler[pResourceRequest->getVmId()] = nSchedulerIndex;

    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR *>(pVmSchedulerModule);
    int nManagedCpuCores = pResourceRequest->getTotalCpus();

    bool *isCPU_Idle = new bool[nManagedCpuCores];
    for (int i = 0; i < nManagedCpuCores; i++)
        isCPU_Idle[i] = true;

    pVmScheduler->setManagedCpuCores(nManagedCpuCores);
    pVmScheduler->setCpuCoreIndex(cpuCoreIndex);
    pVmScheduler->setIsCpuIdle(isCPU_Idle);
    pVmScheduler->setIsRunning(true);

    return pVmAppVectorModule;
}

void Hypervisor::deallocateVmResources(std::string vmId)
{
    // This could be fixed with an approach similar to AppControlBlock
    NodeResourceRequest *pResourceRequest = getOrNull(mapResourceRequestPerVm, vmId);
    int nSchedulerIndex = getOrDefault(mapVmScheduler, vmId, -1);

    // If not found
    if (pResourceRequest == nullptr || nSchedulerIndex == -1)
        return;

    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR *>(pCpuSchedArray[nSchedulerIndex]);

    // Extract requested resources
    uint32_t cores = pResourceRequest->getTotalCpus();
    double memory = pResourceRequest->getTotalMemory();
    double disk = pResourceRequest->getTotalDiskGb();
    auto cpuCoreIndex = pVmScheduler->getCpuCoreIndex();

    // Free the resources
    hardwareManager->deallocateResources(cores, memory, disk, cpuCoreIndex);

    // Shutdown scheduler and mark as free
    pVmScheduler->setIsRunning(false);
    freeSchedArray[nSchedulerIndex] = true;
}
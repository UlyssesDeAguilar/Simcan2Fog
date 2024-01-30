#include "CpuSchedulerRR.h"

Define_Module(CpuSchedulerRR);

CpuSchedulerRR::~CpuSchedulerRR()
{
    requestsQueue.clear();
    abortsQueue.clear();
}

void CpuSchedulerRR::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init module parameters
    isVirtualHardware = par("isVirtualHardware");
    numCpuCores = par("numCpuCores");
    quantum = par("quantum");

    // Init the cGates for Hypervisor
    fromHypervisorGate = gate("fromHypervisor");
    toHypervisorGate = gate("toHypervisor");

    // Init the cGates for checking Hub
    fromHubGate = gate("fromHub");
    toHubGate = gate("toHub");

    // Check connections
    if (!toHypervisorGate->getNextGate()->isConnected())
    {
        EV_ERROR << "toHypervisorGate is not connected";
        error("toHypervisorGate is not connected");
    }

    // Init requests queue
    requestsQueue.clear();

    // Non-virtual hardware. Using all the cpu cores
    if (!isVirtualHardware)
    {

        bRunning = true;

        managedCpuCores = numCpuCores;

        // State of CPUs
        isCPU_Idle = new bool[numCpuCores];

        // Init state to idle!
        for (int i = 0; i < numCpuCores; i++)
            isCPU_Idle[i] = true;

        // Index of each CPU core
        cpuCoreIndex = new unsigned int[numCpuCores];

        // Init state to idle!
        for (int i = 0; i < numCpuCores; i++)
            cpuCoreIndex[i] = i;
    }

    // Using virtual hardware.
    else
    {
        bRunning = false;
        isCPU_Idle = nullptr;
        cpuCoreIndex = nullptr;
        managedCpuCores = 0;
    }

    // Show additional data
    if (showInitValues)
    {
        EV_INFO << "isRunning:" << std::boolalpha << bRunning << " - managedCpuCores:" << managedCpuCores << endl;
    }
}

void CpuSchedulerRR::finish()
{

    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *CpuSchedulerRR::getOutGate(cMessage *msg)
{
    // If msg arrives from the Hypervisor
    if (msg->getArrivalGate() == fromHypervisorGate)
        return toHypervisorGate;

    // If msg arrives from the checking hub
    else if (msg->getArrivalGate() == fromHubGate)
    {
        error("This module cannot receive requests from the CPU system!");
    }

    // Msg arrives from an unknown gate
    //        else
    //            error ("Message received from an unknown gate [%s]", msg->getName());
    return nullptr;
}

void CpuSchedulerRR::processSelfMessage(cMessage *msg)
{
    error("This module cannot process self messages:%s", msg->getName());
}

void CpuSchedulerRR::processRequestMessage(SIMCAN_Message *sm)
{
    // Check and cast if it is a CPU request
    auto sm_cpu = check_and_cast<SM_CPU_Message *>(sm);

    if (sm_cpu->getOperation() == SM_AbortCpu)
    {
        if (!deleteFromRequestsQueue(sm))
            abortsQueue.insert(sm);
        else
            delete (sm);
        return;
    }

    // This scheduler is idle
    if (!bRunning)
        error("Scheduler is not running and a request has been received.%s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());

    // Scheduler running...
    EV_DEBUG << "(processRequestMessage) Processing request." << endl
             << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

    // Check the contents
    if ((!sm_cpu->getUseTime()) && (!sm_cpu->getUseMis()))
        error("Empty CPU request.%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());

    // Set quantum and search for an empty cpu core
    sm_cpu->setQuantum(quantum);
    auto cpuIndex = searchIdleCPU();

    // All CPUs are busy
    if (cpuIndex == SC_NotFound)
    {
        EV_DEBUG << "(processRequestMessage) No idle CPU found..." << endl
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
        EV_INFO << "Pushing message to queue..." << endl;

        // Enqueue current computing block
        requestsQueue.insert(sm_cpu);
    }
    else
    {
        // At least, one cpu core is idle

        // Set the CPU core for this request
        sm_cpu->setNextModuleIndex(cpuCoreIndex[cpuIndex]);

        // Update state!
        isCPU_Idle[cpuIndex] = false;

        EV_DEBUG << "(processRequestMessage) CPU idle found..." << endl
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
        EV_INFO << "Sending message to CPU[" << cpuIndex << "]" << endl;

        // Send the request to the CPU processor
        sendRequestMessage(sm_cpu, toHubGate);
    }
}

bool CpuSchedulerRR::deleteFromRequestsQueue(SIMCAN_Message *sm)
{
    // Cast to CPU Message!
    auto sm_cpu = check_and_cast<SM_CPU_Message *>(sm);

    for (cQueue::Iterator iter(requestsQueue); !iter.end(); iter++)
    {
        SM_CPU_Message *msg = (SM_CPU_Message *)*iter;

        // Found
        if (strcmp(msg->getAppInstance(), sm_cpu->getAppInstance()) == 0)
        {
            requestsQueue.remove(msg);
            return true;
        }
    }

    // Not found
    return false;
}

bool CpuSchedulerRR::deleteFromAbortsQueue(SIMCAN_Message *sm)
{
    // Cast to CPU Message!
    auto sm_cpu = check_and_cast<SM_CPU_Message *>(sm);

    for (cQueue::Iterator iter(abortsQueue); !iter.end(); iter++)
    {
        SM_CPU_Message *msg = (SM_CPU_Message *)*iter;
        if (strcmp(msg->getAppInstance(), sm_cpu->getAppInstance()) == 0)
        {
            abortsQueue.remove(msg);
            return true;
        }
    }

    return false;
}

void CpuSchedulerRR::processResponseMessage(SIMCAN_Message *sm)
{
    // Cast
    auto sm_cpu = check_and_cast<SM_CPU_Message *>(sm);

    // Zombie request arrives. This scheduler has been disabled!
    if (!bRunning)
    {
        EV_DEBUG << "(processResponseMessage) Zombie request arrived:" << endl
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
        delete (sm);
        return;
    }
    
    // Update CPU state!
    int realCpuIndex = sm_cpu->getNextModuleIndex();
    int virtualCpuIndex = getVirtualCpuIndex(realCpuIndex);

    // Check bounds
    if ((virtualCpuIndex >= managedCpuCores) || (virtualCpuIndex < 0))
        error("\nCPU index error:%d. There are %d CPUs attached. %s\n", realCpuIndex, managedCpuCores, sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
    else
        isCPU_Idle[virtualCpuIndex] = true;

    EV_DEBUG << "(processResponseMessage) Arrives a message from CPU core:" << realCpuIndex << endl
             << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

    if (!sm_cpu->isCompleted())
    {
        // Current computing block has not been completely executed
        EV_INFO << "Request CPU not completed... Pushing to queue." << endl;
        SM_CPU_Message *sm_cpu_status = sm_cpu->dup();
        sm_cpu_status->setIsResponse(true);
        sendResponseMessage(sm_cpu_status);

        sm_cpu->setIsResponse(false);
        requestsQueue.insert(sm_cpu);
    }

    // Current block has been completely executed
    else
    {
        EV_INFO << "Request CPU completed. Sending it back to the application." << endl;
        // sm_cpu->setResult(SM_APP_Res_Accept);
        sm_cpu->setIsResponse(true);
        sendResponseMessage(sm_cpu);
    }

    // Empty queue
    if (requestsQueue.isEmpty())
    {
        EV_DEBUG << "(processResponseMessage) Empty queue!" << endl;
        return;
    }

    // There are pending requests
    // Pop and cast
    auto unqueuedMessage = (cMessage *)requestsQueue.pop();
    auto nextRequest = check_and_cast<SIMCAN_Message *>(unqueuedMessage);

    // Set the cpu core index
    nextRequest->setNextModuleIndex(realCpuIndex);

    // Update state of CPU and prepare quantum
    isCPU_Idle[virtualCpuIndex] = false;
    auto sm_cpuNext = check_and_cast<SM_CPU_Message *>(nextRequest);
    sm_cpuNext->setQuantum(quantum);

    // Debug
    EV_DEBUG << "(processResponseMessage) CPU idle found..." << endl
             << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
    EV_INFO << "Sending message to CPU[" << realCpuIndex << "]" << endl;

    // Send!
    sendRequestMessage(nextRequest, toHubGate);
}

int CpuSchedulerRR::getVirtualCpuIndex(unsigned int realCpuIndex)
{
    for (unsigned int i = 0; i < managedCpuCores; i++)
        if (cpuCoreIndex[i] == realCpuIndex)
            return i;
    return -1;
}
unsigned int *CpuSchedulerRR::getCpuCoreIndex() const
{
    return cpuCoreIndex;
}

void CpuSchedulerRR::setCpuCoreIndex(unsigned int *cpuCoreIndex)
{
    this->cpuCoreIndex = cpuCoreIndex;
}

bool CpuSchedulerRR::isRunning() const
{
    return bRunning;
}

void CpuSchedulerRR::setIsRunning(bool bRunning)
{
    if (this->bRunning && !bRunning)
        stopAllProcess();
    this->bRunning = bRunning;
}

unsigned int CpuSchedulerRR::getManagedCpuCores() const
{
    return managedCpuCores;
}

void CpuSchedulerRR::setManagedCpuCores(unsigned int managedCpuCores)
{
    this->managedCpuCores = managedCpuCores;
}

bool *CpuSchedulerRR::getIsCpuIdle() const
{
    return isCPU_Idle;
}

void CpuSchedulerRR::setIsCpuIdle(bool *isCpuIdle)
{
    isCPU_Idle = isCpuIdle;
}

int CpuSchedulerRR::searchIdleCPU()
{
    for (int i = 0; i < managedCpuCores; i++)
        if (isCPU_Idle[i])
            return i;

    return SC_NotFound;
}

void CpuSchedulerRR::stopAllProcess()
{
    requestsQueue.clear();
    for (unsigned int i = 0; i < managedCpuCores; i++)
        if (!isCPU_Idle[i])
            stopCpu(i);
}

void CpuSchedulerRR::stopCpu(unsigned int virtualCpuIndex)
{
    // Creates the message
    auto sm_cpu = new SM_CPU_Message();
    take(sm_cpu);
    sm_cpu->setOperation(SM_AbortCpu);

    // Update message length
    sm_cpu->updateLength();

    // Set the CPU core for this request
    sm_cpu->setNextModuleIndex(cpuCoreIndex[virtualCpuIndex]);

    // Send the request to the CPU processor
    sendRequestMessage(sm_cpu, toHubGate);
}

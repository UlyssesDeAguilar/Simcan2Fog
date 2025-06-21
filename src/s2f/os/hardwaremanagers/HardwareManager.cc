#include "HardwareManager.h"
using namespace s2f::os;

Register_Abstract_Class(HardwareManager);

void HardwareManager::initialize()
{
    // Obtain the parameters of the module
    isVirtualHardware = par("isVirtualHardware");

    // Init the free core list
    coresState.resize(getTotalResources().cores);
}

void HardwareManager::finish()
{
    simtime_t finalTime = simTime();
    uint32_t coreIndex = 0;
    for (auto &core : coresState)
    {
        // Special case where the sim was interrupted but the cpus were still allocated
        if (!core.free)
            core.usageTime += (simTime().inUnit(SIMTIME_S) - core.startTime);

        EV << "core[" << coreIndex++ << "]: "
           << "utilization time: " << core.usageTime
           << " proportion " << core.usageTime / finalTime << "\n";
    }
}

void HardwareManager::handleMessage(cMessage *msg)
{
    delete msg;
    error("This module doesn't receive messages");
}

void HardwareManager::checkNodeSetup(const NodeResources &resources)
{
    // Check consistency (cloud environments)
    if (isVirtualHardware)
    {

        if ((resources.vms <= 0) || (resources.vms > resources.cores))
            error("For virtual environments, 0 < maxVMs <= numCpuCores");

        if (resources.cores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (resources.users != resources.vms)
            error("For virtual environments, maxUsers = maxVMs");
    }
    // Check consistency (cluster environments)
    else
    {
        if (resources.vms != 1)
            error("For non-virtual environments, maxVMs must be 1");

        if (resources.cores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (resources.users <= 0)
            error("The parameter maxUsers must be a positive value (>0)");
    }
}

void HardwareManager::allocateCores(int cores, uint32_t **const coreIndex)
{
    *coreIndex = nullptr;

    // Prepare the index
    uint32_t *cpuCoreIndex = new uint32_t[cores]{UINT32_MAX};

    for (int i = 0, j = 0; j < cores && i < getTotalResources().cores; i++)
    {
        // Keep going until we get a free slot
        if (coresState[i].free)
        {
            // Register position and mark allocated
            cpuCoreIndex[j] = i;
            coresState[i].free = false;
            coresState[i].startTime = simTime().inUnit(SIMTIME_S);
            // Jump to next index
            j++;
        }
    }

    // Give back the index
    *coreIndex = cpuCoreIndex;
}

void HardwareManager::deallocateCores(const uint32_t *coreIndex, size_t coreIndexSize)
{
    // Release marked cores
    for (int i = 0; i < coreIndexSize; i++)
    {
        int index = coreIndex[i];
        coresState[index].free = true;
        coresState[index].usageTime += (simTime().inUnit(SIMTIME_S) - coresState[index].startTime);
    }
}

#include "HardwareManager.h"

Define_Module(HardwareManager);

void HardwareManager::initialize()
{
    // Obtain the node Ip
    nodeIp = par("address");

    // Obtain the parameters of the module
    isVirtualHardware = par("isVirtualHardware");

    // Init the resource specs
    total.cores = par("numCpuCores");
    total.memory = par("memorySize");
    total.disk = par("diskSize");
    total.vms = par("maxVMs");
    total.users = par("maxUsers");

    // Init the available specs
    available = total;

    // Init the free core list
    coresState.resize(total.cores);

    // Check consistency (cloud environments)
    if (isVirtualHardware)
    {

        if ((total.vms <= 0) || (total.vms > total.cores))
            error("For virtual environments, 0 < maxVMs <= numCpuCores");

        if (total.cores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (total.users != total.vms)
            error("For virtual environments, maxUsers = maxVMs");
    }
    // Check consistency (cluster environments)
    else
    {
        if (total.vms != 1)
            error("For non-virtual environments, maxVMs must be 1");

        if (total.cores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (total.users <= 0)
            error("The parameter maxUsers must be a positive value (>0)");
    }
}

void HardwareManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

bool HardwareManager::tryAllocateResources(const uint32_t &cores, const double &memory, const double &disk, uint32_t **const coreIndex)
{
    // Initialize
    *coreIndex = nullptr;

    // Check availability of vms
    if (available.vms == 0)
        return false;

    // Check availability of the resources
    if (memory > available.memory || disk > available.disk || cores > available.cores)
        return false;

    // Prepare the index
    uint32_t *cpuCoreIndex = new uint32_t[cores]{UINT32_MAX};

    // Search the free slots
    for (int i = 0, j = 0; i < cores && j < total.cores; j++)
    {
        // Keep going until we get a free slot
        if (coresState[j].free)
        {
            // Register position and mark allocated
            cpuCoreIndex[i] = j;
            coresState[j].free = false;
            coresState[j].startTime = simTime().inUnit(SIMTIME_S);
            // Jump to next index
            i++;
        }
    }

    // Mark as allocated
    available.cores -= cores;
    available.memory -= memory;
    available.disk -= disk;
    available.vms--;

    // Give back the index
    *coreIndex = cpuCoreIndex;

    return true;
}

void HardwareManager::deallocateResources(const uint32_t &cores, const double &memory, const double &disk, const uint32_t *coreIndex)
{
    // Release vm "slot"
    available.vms++;

    // Release memory, disk, and cores
    available.memory += memory;
    available.disk += disk;
    available.cores += cores;

    // Release marked cores
    for (int i = 0; i < cores; i++)
    {
        int index = coreIndex[i];
        coresState[index].free = true;
        coresState[index].usageTime += (simTime().inUnit(SIMTIME_S) - coresState[index].startTime);
    }
}

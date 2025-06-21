#include "s2f/os/hardwaremanagers/EdgeHardwareManager.h"

using namespace omnetpp;
using namespace s2f::os;

Define_Module(EdgeHardwareManager);

void EdgeHardwareManager::initialize()
{
    nodeIp = par("address");
    total.cores = par("numCpuCores");
    total.memoryMiB = uint32_t(std::ceil(par("memorySize").doubleValue()));
    total.diskMiB = uint32_t(std::ceil(par("diskSize").doubleValue()));
    total.vms = par("maxVMs");
    total.users = par("maxUsers");
    available = total;
    
    HardwareManager::initialize();
    checkNodeSetup(getTotalResources());
}

bool EdgeHardwareManager::tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex)
{
    // Initialize
    int cores = vmTemplate->getNumCores();

    // Check availability of the resources
    if (!(*vmTemplate <= available))
        return false;

    allocateCores(cores, coreIndex);

    // Mark as allocated
    available.cores -= cores;
    available.memoryMiB -= vmTemplate->getMemoryMiB();
    available.diskMiB -= vmTemplate->getDiskMiB();
    available.vms--;
    
    return true;
}


void EdgeHardwareManager::deallocateResources(const VirtualMachine* vmTemplate, const uint32_t *coreIndex)
{
    // Release vm "slot"
    available.vms++;

    // Release memory, disk, and cores
    available.memoryMiB += vmTemplate->getMemoryMiB();
    available.diskMiB += vmTemplate->getDiskMiB();
    available.cores += vmTemplate->getNumCores();

    deallocateCores(coreIndex, vmTemplate->getNumCores());
}
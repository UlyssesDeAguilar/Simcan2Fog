#include "s2f/os/hardwaremanagers/DcHardwareManager.h"

using namespace omnetpp;
Define_Module(DcHardwareManager);

void DcHardwareManager::initialize(int stage)
{
    if (stage == SimCanInitStages::LOCAL)
    {
        resourceManager = check_and_cast<ResourceManager *>(getModuleByPath(par("resourceManagerPath")));
        networkCard = check_and_cast<NetworkCard *>(getModuleByPath(par("networkCardPath")));
    }
    else if (stage == SimCanInitStages::NEAR)
    {
        // Empty (for now)
    }
    else if (stage == SimCanInitStages::BLADE)
    {
        total.memoryMiB = uint32_t(std::ceil(par("memorySize").doubleValue()));
        total.diskMiB = uint32_t(std::ceil(par("diskSize").doubleValue()));
        total.cores = par("numCpuCores");
        total.vms = par("maxVMs");
        total.users = par("maxUsers");
        HardwareManager::initialize();
        checkNodeSetup(total);

        // Register node
        nodeId = resourceManager->addNode(getIp(), total);
    }
}

bool DcHardwareManager::tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex)
{
    allocateCores(vmTemplate->getNumCores(), coreIndex);
    return true;
}

void DcHardwareManager::deallocateResources(const VirtualMachine *vmTemplate, const uint32_t *coreIndex)
{
    deallocateCores(coreIndex, vmTemplate->getNumCores());
    resourceManager->confirmNodeDeallocation(nodeId, vmTemplate);
}

int DcHardwareManager::getIp() const { return networkCard->par("L2Address"); }

const NodeResources &DcHardwareManager::getTotalResources() const { return total; }

const NodeResources &DcHardwareManager::getAvailableResources() const { return resourceManager->getNodeAvailableResources(nodeId); }

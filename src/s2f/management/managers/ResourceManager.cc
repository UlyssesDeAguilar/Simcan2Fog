#include "ResourceManager.h"
#include "s2f/core/include/signals.h"

using namespace omnetpp;

Define_Module(ResourceManager);

void ResourceManager::initialize(int stage)
{
    if (stage == LOCAL)
    {
        machinesInUse = 0;
        activeMachines = 0;
        minActiveMachines = 0;
        defaultNodePool = check_and_cast<NodePool *>(findModuleByPath(par("nodePoolPath")));
        selectionStrategy = check_and_cast<dc::SelectionStrategy *>(createOne(par("selectionStrategy")));
        dataManager = check_and_cast<DataManager *>(getModuleByPath(par("dataManagerPath")));
    }
    else if (stage == MANAGER)
    {
        this->activeMachines = 0;
        this->machinesInUse = 0;

        // Emit the overall registered resources
        auto &resources = defaultNodePool->getTotalResources();
        emit(maxCores, resources.cores);
        emit(maxDisk, resources.diskMiB / 1024.0);
        emit(maxRam, resources.memoryMiB / 1024.0);
        emit(maxVms, resources.vms);
    }
}

void ResourceManager::finish()
{
    // Cpu usage times! -> emit signal instad of printing? Global cpu view like the hypervisor?
    // HardwareManager should "expose" the cpuState array!

    // Give the allocation signals a ending point
    // emit(signals.allocatedCores, 0);
    // emit(signals.allocatedVms, 0);
    // emit(signals.allocatedDisk, 0.0);
    // emit(signals.allocatedRam, 0.0);
}

uint64_t ResourceManager::getAvailableCores() const { return defaultNodePool->getTotalResources().cores; }
uint64_t ResourceManager::getAvailableDiskMiB() const { return defaultNodePool->getTotalResources().diskMiB; }
uint64_t ResourceManager::getAvailableRamMiB() const { return defaultNodePool->getTotalResources().memoryMiB; }

double ResourceManager::getAggregateCpuUsage() const
{
    PoolResources total = defaultNodePool->getTotalResources();
    PoolResources available = defaultNodePool->getAvailableResources();
    return ((total.cores - available.cores) / (double)total.cores) * 100.0;
}

size_t ResourceManager::addNode(int adress, const NodeResources &resources)
{
    Enter_Method_Silent();
    return defaultNodePool->addNode(adress, resources);
}

void ResourceManager::setActiveMachines(uint32_t activeMachines) { error("To be reimplemented"); }

hypervisor::DcHypervisor *const ResourceManager::getHypervisor(uint32_t nodeIp)
{
    cModule *blade = getParentModule()->getSubmodule("blade", nodeIp);
    cModule *hypervisor = blade->getSubmodule("osModule")->getSubmodule("hypervisor");
    return check_and_cast<hypervisor::DcHypervisor *>(hypervisor);
}

void ResourceManager::emitSignals(const VirtualMachine *vmTemplate, bool allocation)
{
    // Emit statistical signals -- Heuristic for considering if there may be a listening configuration
    if (!mayHaveListeners(allocatedVms))
        return;

    if (allocation)
    {
        emit(allocatedVms, 1);
        emit(allocatedCores, vmTemplate->getNumCores());
        emit(allocatedRam, vmTemplate->getMemoryMiB() / 1024.0);
        emit(allocatedDisk, vmTemplate->getDiskMiB() / 1024.0);
    }
    else
    {
        emit(allocatedVms, -1);
        emit(allocatedCores, -vmTemplate->getNumCores());
        emit(allocatedRam, -vmTemplate->getMemoryMiB() / 1024.0);
        emit(allocatedDisk, -vmTemplate->getMemoryMiB() / 1024.0);
    }
}

void ResourceManager::confirmNodeDeallocation(size_t nodeId, const VirtualMachine *vmTemplate)
{
    defaultNodePool->deallocateResources(nodeId, vmTemplate);
    emitSignals(vmTemplate, false);
}

const NodeResources &ResourceManager::getNodeAvailableResources(size_t nodeId) const
{
    return defaultNodePool->getNode(nodeId).availableResources;
}

SM_UserVM *ResourceManager::allocateVms(SM_UserVM *request)
{
    // This record table will hold the relation vm <-> node
    struct NodeVmRecord
    {
        size_t nodeIndex;
        const VirtualMachine *vmTemplate;
    };

    std::vector<NodeVmRecord> allocRecords;
    uint64_t cores, ram, disk = 0;
    bool ok = true;

    for (int i = 0, n = request->getVmArraySize(); i < n && ok; i++)
    {
        size_t nodeIndex;
        VM_Request &vm = request->getVmForUpdate(i);
        const VirtualMachine *vmTemplate = dataManager->searchVm(vm.vmType);

        if (!selectionStrategy->selectNode(vmTemplate, defaultNodePool->getNodes(), nodeIndex))
            ok = false;
        else
        {
            // Also add it as a response
            defaultNodePool->allocateResources(nodeIndex, vmTemplate);
            allocRecords.push_back({nodeIndex, vmTemplate});

            // Accumulate
            cores += vmTemplate->getNumCores();
            ram += vmTemplate->getMemoryMiB();
            disk += vmTemplate->getDiskMiB();
        }
    }

    // In case of failure, restore the resources
    if (!ok)
        for (const auto record : allocRecords)
            defaultNodePool->deallocateResources(record.nodeIndex, record.vmTemplate);
    else
    {
        // Emit the allocation signals
        emit(allocatedVms, request->getVmArraySize());
        emit(allocatedCores, cores);
        emit(allocatedRam, ram / 1024.0);
        emit(allocatedDisk, disk / 1024.0);
    }

    return ok ? request : nullptr;
}
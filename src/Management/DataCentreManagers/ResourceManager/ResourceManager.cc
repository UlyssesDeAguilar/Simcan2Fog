#include "ResourceManager.h"
using namespace omnetpp;

Define_Module(DcResourceManager);

void DcResourceManager::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
        // Register the signals for statistical recording
        signals.maxCores = registerSignal("maxCores");
        signals.maxRam = registerSignal("maxRam");
        signals.maxDisk = registerSignal("maxDisk");
        signals.maxVms = registerSignal("maxVms");

        signals.allocatedCores = registerSignal("allocatedCores");
        signals.allocatedRam = registerSignal("allocatedRam");
        signals.allocatedDisk = registerSignal("allocatedDisk");
        signals.allocatedVms = registerSignal("allocatedVms");

        // Reserve the necessary control data
        int numBlades = getParentModule()->par("numBlades");
        nodes.resize(numBlades);
        break;
    }
    case MANAGER:
    {
        // Initialize buckets and reservations
        initBucketsAndReservations();
        // Mark all cores as available
        availableCores = totalCores;
        break;
    }
    default:
        break;
    }
}

void DcResourceManager::finish()
{
    // Cpu usage times! -> emit signal instad of printing? Global cpu view like the hypervisor?
    // HardwareManager should "expose" the cpuState array!

    // Give the allocation signals a ending point
    emit(signals.allocatedCores, 0);
    emit(signals.allocatedVms, 0);
    emit(signals.allocatedDisk, 0.0);
    emit(signals.allocatedRam, 0.0);
}

void DcResourceManager::registerNode(uint32_t ip, const NodeResources &resources)
{
    // Insert the hypervisor to the correct bucket
    totalCores += resources.cores;

    // Register state
    nodes[ip] = {.availableResources = resources, .ip = ip, .state = 0};
}

void DcResourceManager::initBucketsAndReservations()
{
    auto registerNode = [](CoreHypervisorsMap &map, Node &node) -> void
    {
        map[node.availableResources.cores].push_back(node.ip);
    };

    this->activeMachines = 0;
    this->machinesInUse = 0;
    uint32_t reservedCounter = 0;

    // For statistics
    double maxRam = 0.0;
    double maxDisk = 0.0;
    uint64_t maxVMs = 0;

    for (auto &node : nodes)
    {
        // Accumulate
        maxRam += node.availableResources.memory;
        maxDisk += node.availableResources.disk;
        maxVMs += node.availableResources.vms;

        // Activate the node
        if (this->activeMachines < this->minActiveMachines)
            activateNode(node.ip);

        if (reservedCounter < this->reservedNodes)
        {
            // Mark as reserved and store in correct bucket
            node.state |= Node::RESERVED;
            registerNode(reservedCoresHypervisor, node);
            reservedCounter++;
        }
        else
            registerNode(coresHypervisor, node);
    }

    // Sanity checks
    if (reservedCounter < this->reservedNodes)
        error("Unable to reserve %d nodes because there are %d total or active nodes", this->reservedNodes, nodes.size());

    if (this->activeMachines < this->minActiveMachines)
        error("Unable to activate the minimum %d required nodes because there are %d total nodes", this->minActiveMachines, nodes.size());

    // Emit the overall registered resources
    emit(signals.maxCores, totalCores);
    emit(signals.maxDisk, maxDisk);
    emit(signals.maxRam, maxRam);
    emit(signals.maxVms, maxVMs);

    // Give the allocation signals a starting point
    emit(signals.allocatedCores, 0);
    emit(signals.allocatedVms, 0);
    emit(signals.allocatedDisk, 0.0);
    emit(signals.allocatedRam, 0.0);
}

void DcResourceManager::setActiveMachines(uint32_t activeMachines)
{
    // Bounds check
    if (activeMachines < minActiveMachines)
        activeMachines = minActiveMachines;

    // Scale up or scale down
    if (this->activeMachines < activeMachines)
    {
        // Start from the lowest core count hypervisors
        for (auto &bucket : coresHypervisor)
        {
            for (auto &nodeIp : bucket.second)
            {
                if (!nodes[nodeIp].isActive())
                    activateNode(nodeIp);

                // Check if we hit our target
                if (this->activeMachines >= activeMachines)
                    return;
            }
        }
    }
    else if (this->activeMachines > activeMachines)
    {
        // Start from the highest core count hypervisors
        for (auto ritMap = coresHypervisor.rbegin(); ritMap != coresHypervisor.rend(); ++ritMap)
        {
            for (auto &nodeIp : ritMap->second)
            {
                if (!nodes[nodeIp].inUse())
                    deactivateNode(nodeIp);

                // Check if we hit our target
                if (this->activeMachines <= activeMachines)
                    return;
            }
        }
    }
}

void DcResourceManager::activateNode(uint32_t nodeIp)
{
    getHypervisor(nodeIp)->powerOn(true);
    nodes[nodeIp].state |= Node::ACTIVE;
    this->activeMachines++;
}

void DcResourceManager::deactivateNode(uint32_t nodeIp)
{
    getHypervisor(nodeIp)->powerOn(false);
    nodes[nodeIp].state &= ~(Node::ACTIVE);
    this->activeMachines--;
}

hypervisor::DcHypervisor *const DcResourceManager::getHypervisor(uint32_t nodeIp)
{
    cModule *blade = getParentModule()->getSubmodule("blade", nodeIp);
    cModule *hypervisor = blade->getSubmodule("osModule")->getSubmodule("hypervisor");
    return check_and_cast<hypervisor::DcHypervisor *>(hypervisor);
}

void DcResourceManager::confirmNodeAllocation(const uint32_t &ip, const VirtualMachine *vmTemplate)
{
    this->availableCores -= vmTemplate->getNumCores();

    // Emit statistical signals -- Heuristic for considering if there may be a listening configuration
    if (mayHaveListeners(signals.allocatedVms))
    {
        emit(signals.allocatedVms, 1);
        emit(signals.allocatedCores, vmTemplate->getNumCores());
        emit(signals.allocatedRam, vmTemplate->getMemoryGb());
        emit(signals.allocatedDisk, vmTemplate->getDiskGb());
    }

    // Mark node in use and increment machinesInUse (if previously not set)
    if ((nodes[ip].state & Node::IN_USE) == 0)
    {
        nodes[ip].state |= Node::IN_USE;
        machinesInUse++;
    }

    // Check if the node is exhausted
    if (nodes[ip].availableResources.isExhausted())
        nodes[ip].state |= Node::MAXED_OUT;
}

void DcResourceManager::confirmNodeDeallocation(const uint32_t &ip, const VirtualMachine *vmTemplate, bool idleNode)
{
    this->availableCores += vmTemplate->getNumCores();

    // Emit statistical signals -- Heuristic for considering if there may be a listening configuration
    if (mayHaveListeners(signals.allocatedVms))
    {
        emit(signals.allocatedVms, -1);
        emit(signals.allocatedCores, -vmTemplate->getNumCores());
        emit(signals.allocatedRam, -vmTemplate->getMemoryGb());
        emit(signals.allocatedDisk, -vmTemplate->getDiskGb());
    }

    // If no longer in use
    if (idleNode)
    {
        nodes[ip].state &= ~(Node::IN_USE);
        machinesInUse--;
    }

    // Relocate the resources -> update the buckets!
    updateNode(ip, vmTemplate, false);

    // Check if the node is no longer exhausted
    if (!nodes[ip].availableResources.isExhausted())
        nodes[ip].state &= ~(Node::MAXED_OUT);
}

void DcResourceManager::updateNode(uint32_t nodeIp, const VirtualMachine *vm, bool allocate)
{
    // Recover resources
    NodeResources &resources = getNodeResources(nodeIp);

    // Recover bucket
    auto bucketIter = coresHypervisor.find(resources.cores);
    std::vector<uint32_t> &oldBucket = bucketIter->second;

    // Swap with last element in bucket and pop back O(1)
    auto pos = std::find(oldBucket.begin(), oldBucket.end(), nodeIp);
    std::swap(*pos, oldBucket.back());
    oldBucket.pop_back();

    // If there are no more nodes, then delete the bucket
    if (oldBucket.size() == 0)
        coresHypervisor.erase(bucketIter);
    
    if (allocate)
    {
        // Allocate the resources
        resources.cores -= vm->getNumCores();
        resources.disk -= vm->getDiskGb();
        resources.memory -= vm->getMemoryGb();
        resources.vms--;
    }
    else
    {
        // Deallocate the resources
        resources.cores += vm->getNumCores();
        resources.disk += vm->getDiskGb();
        resources.memory += vm->getMemoryGb();
        resources.vms++;
    }

    // Push in the correct bucket
    std::vector<uint32_t> &newBucket = coresHypervisor[resources.cores];
    newBucket.push_back(nodeIp);
}

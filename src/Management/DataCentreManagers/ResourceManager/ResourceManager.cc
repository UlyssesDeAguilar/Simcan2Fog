#include "ResourceManager.h"
using namespace omnetpp;

Define_Module(DcResourceManager);

void DcResourceManager::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
        // Reserve the necessary control data
        int numBlades = getParentModule()->par("numBlades");
        nodes.reserve(numBlades);
        break;
    }
    case MANAGER:
    {
        // Initialize buckets and reservations
        initBucketsAndReservations();
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
        node.bucketIndex = map[node.availableResources.cores].size() - 1;
    };

    this->activeMachines = 0;
    this->machinesInUse = 0;
    uint32_t reservedCounter = 0;

    for (auto &node : nodes)
    {
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
        error("Unable to reserve %d nodes because there are %d total or active nodes", this->reservedNodes, reservedCounter);

    if (this->activeMachines < this->minActiveMachines)
        error("Unable to activate the minimum %d required nodes because there are %d total nodes", this->minActiveMachines, reservedCounter);
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
    cModule *blade = getParentModule()->getSubmodule("blades", nodeIp);
    cModule *hypervisor = blade->getSubmodule("osModule")->getSubmodule("hypervisor");
    return check_and_cast<hypervisor::DcHypervisor *>(hypervisor);
}

void DcResourceManager::confirmNodeAllocation(const uint32_t &ip, const VirtualMachine *vmTemplate)
{
    // TODO: Statistics update!
    this->availableCores -= vmTemplate->getNumCores();

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

void DcResourceManager::confirmNodeDeallocation(const uint32_t &ip, const VirtualMachine *vmTemplate, bool inUse)
{
    // TODO: Statistics update!
    this->availableCores += vmTemplate->getNumCores();

    // If no longer in use
    if (!inUse)
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
    auto &resources = getNodeResources(nodeIp);

    // Recover bucket
    auto oldBucket = coresHypervisor.at(resources.cores);

    // Swap with last element in bucket and pop back O(1)
    std::swap(oldBucket.at(nodes[nodeIp].bucketIndex), oldBucket.back());
    oldBucket.pop_back();

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
    auto &newBucket = coresHypervisor[resources.cores];
    newBucket.push_back(nodeIp);
    nodes[nodeIp].bucketIndex = newBucket.size() - 1;
}

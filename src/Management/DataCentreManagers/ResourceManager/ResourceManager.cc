#include "ResourceManager.h"
using namespace omnetpp;

Define_Module(DcResourceManager);

void DcResourceManager::initialize(int stage)
{
    // TODO
}

void DcResourceManager::registerNode(uint32_t ip, const NodeResources &resources, bool isActive)
{
    uint16_t flags = 0;

    // Insert the hypervisor to the correct bucket
    coresHypervisor[resources.cores].push_back(ip);
    totalCores += resources.cores;

    // If active
    if (isActive)
    {
        activeMachines++;
        flags |= Node::ACTIVE;
    }

    // Register state
    nodes[ip] = {.availableResources = resources, .ip = ip, .state = flags};
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
                if (!nodes[nodeIp].isActive())
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
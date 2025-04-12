#include "s2f/management/managers/NodePool.h"

using namespace omnetpp;

Define_Module(NodePool);

void NodePool::initialize()
{
    ssize_t numNodes = par("estimatedNumberOfNodes");
    if (numNodes > 0)
        nodes.reserve(numNodes);
}

void NodePool::finish()
{
    nodes.clear();
    totalResources.reset();
    availableResources.reset();
}

void NodePool::handleMessage(cMessage *msg)
{
    delete msg;
    error("This module cannot handle messages");
}

size_t NodePool::addNode(int address, const NodeResources &resources)
{
    Enter_Method_Silent("Adding node with address %d", address);
    size_t index = nodes.size();
    nodes.emplace_back();
    Node &node = nodes.back();

    // Set values
    node.address = address;
    node.availableResources = resources;
    node.setActive();

    // Update total resources
    totalResources.memoryMiB += resources.memoryMiB;
    totalResources.diskMiB += resources.diskMiB;
    totalResources.cores += resources.cores;
    totalResources.vms += resources.vms;

    // Update available resources
    availableResources = totalResources;

    EV_INFO << "Node " << index << " added: " << node << "\n";

    return index;
}

void NodePool::allocateResources(Node &node, const VirtualMachine *vmTemplate)
{
    NodeResources &nodeResources = node.availableResources;
    nodeResources.memoryMiB -= vmTemplate->getMemoryMiB();
    nodeResources.diskMiB -= vmTemplate->getDiskMiB();
    nodeResources.cores -= vmTemplate->getNumCores();
    nodeResources.vms--;
}

void NodePool::deallocateResources(Node &node, const VirtualMachine *vmTemplate)
{
    NodeResources &nodeResources = node.availableResources;
    nodeResources.memoryMiB += vmTemplate->getMemoryMiB();
    nodeResources.diskMiB += vmTemplate->getDiskMiB();
    nodeResources.cores += vmTemplate->getNumCores();
    nodeResources.vms++;
}
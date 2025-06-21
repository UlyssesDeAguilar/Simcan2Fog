#ifndef SIMCAN_EX_NODE_POOL_H_
#define SIMCAN_EX_NODE_POOL_H_

#include <omnetpp.h>
#include "s2f/management/managers/Node.h"

struct PoolResources
{
    uint64_t memoryMiB{};
    uint64_t diskMiB{};
    uint64_t cores{};
    uint64_t vms{};
    void reset() { memoryMiB = diskMiB = cores = vms = 0; }
};

/**
 * @class NodePool
 * @brief Manages a pool of nodes and their resources.
 */
class NodePool : public omnetpp::cSimpleModule
{
protected:
    std::vector<Node> nodes; //!< Table of nodes
    PoolResources totalResources{};
    PoolResources availableResources{};

    virtual void initialize() override;

    virtual void finish() override;

    virtual void handleMessage(omnetpp::cMessage *msg) override;

public:
    /**
     * Returns a reference to a node at the specified index.
     *
     * @param index The index of the node to retrieve.
     * @return A const reference to the node at the specified index.
     */
    const Node &getNode(int index) const { return nodes.at(index); }

    const std::vector<Node> &getNodes() const { return nodes; }

    /**
     * Returns the number of nodes in the pool.
     *
     * @return The number of nodes in the pool.
     */
    size_t getNumNodes() const { return nodes.size(); }

    /**
     * Adds a new node to the pool.
     *
     * @param address The address of the node.
     * @param hostname The hostname of the node.
     * @param resources The resources available on the node.
     * @return The index of the newly added node.
     */
    size_t addNode(int address, const s2f::os::NodeResources &resources);

    /**
     * Allocates resources on a node for a virtual machine.
     *
     * @param node The node to allocate resources on.
     * @param vmTemplate The virtual machine template.
     */
    void allocateResources(Node &node, const VirtualMachine *vmTemplate);

    void allocateResources(size_t index, const VirtualMachine *vmTemplate) { allocateResources(nodes.at(index), vmTemplate); }

    /**
     * Deallocates resources on a node for a virtual machine.
     *
     * @param node The node to deallocate resources on.
     * @param vmTemplate The virtual machine template.
     */
    void deallocateResources(Node &node, const VirtualMachine *vmTemplate);

    void deallocateResources(size_t index, const VirtualMachine *vmTemplate) { deallocateResources(nodes.at(index), vmTemplate); }

    const PoolResources &getTotalResources() const { return totalResources; }
    const PoolResources &getAvailableResources() const { return availableResources; }
};

#endif /* SIMCAN_EX_NODE_POOL_H_ */

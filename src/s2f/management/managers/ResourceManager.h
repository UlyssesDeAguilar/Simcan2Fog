#ifndef SIMCAN_EX_DC_RESOURCE_MANAGER
#define SIMCAN_EX_DC_RESOURCE_MANAGER

#include "omnetpp.h"

// #include "s2f/architecture/nodes/hardwaremanagers/HardwareManager.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/os/hypervisors/DcHypervisor.h"
#include "s2f/management/managers/NodePool.h"
#include "s2f/core/simdata/DataManager.h"
#include "s2f/management/managers/selection/Strategies.h"

class ResourceManager : public omnetpp::cSimpleModule
{
protected:
    NodePool *defaultNodePool{};                //!< Default node pool
    DataManager *dataManager{};                 //!< Data manager
    dc::SelectionStrategy *selectionStrategy{}; //!< Selection strategy
    uint32_t machinesInUse{};                   //!< Nodes in use
    uint32_t activeMachines{};                  //!< Active nodes (powered on)
    uint32_t minActiveMachines{};               //!< Minimum required active nodes

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return TOTAL_INIT_STAGES; }
    virtual void finish() override;

    /**
     * @brief Get the Hypervisor
     * @param nodeIp The node ip where the hypervisor resides
     * @return hypervisor::DcHypervisor* const
     */
    hypervisor::DcHypervisor *const getHypervisor(uint32_t nodeIp);

public:
    struct NodeVmRecord
    {
        size_t nodeIndex;
        const VirtualMachine *vmTemplate;
    };

    void emitSignals(const VirtualMachine *vmTemplate, bool allocation);

    /**
     * @brief Register a node of the dc
     * @param ip Ip of the node
     * @param resources The resources of the node
     */
    size_t addNode(int address, const NodeResources &resources);
    void confirmNodeDeallocation(size_t nodeId, const VirtualMachine *vmTemplate);
    const NodeResources &getNodeAvailableResources(size_t nodeId) const;
    const Node &getNode(size_t nodeId) const { return defaultNodePool->getNode(nodeId); }
    const std::vector<NodeVmRecord> *allocateVms(SM_UserVM *request);

    // void resumeVm(uint32_t ip, const char *vmId, int extensionTime) { getHypervisor(ip)->extendVm(std::string(vmId), extensionTime); }
    // void deallocateVm(uint32_t ip, const char *vmId) { getHypervisor(ip)->deallocateVmResources(std::string(vmId)); }

    /**
     * @brief Scans through the nodes and activates or deactivates them to match the objective count
     * @param activeMachines
     */
    void setActiveMachines(uint32_t activeMachines);
    void setMinActiveMachines(uint32_t minimum) { minActiveMachines = minimum; }

    // Statistical and management information
    uint32_t getMachinesInUse() const { return machinesInUse; }
    uint32_t getActiveMachines() const { return activeMachines; }
    uint32_t getActiveOrInUseMachines() const { return machinesInUse + activeMachines; }

    size_t getTotalNodes() const;
    uint64_t getAvailableCores() const;
    uint64_t getAvailableDiskMiB() const;
    uint64_t getAvailableRamMiB() const;
    double getAggregateCpuUsage() const;
};

#endif /* SIMCAN_EX_DC_RESOURCE_MANAGER */

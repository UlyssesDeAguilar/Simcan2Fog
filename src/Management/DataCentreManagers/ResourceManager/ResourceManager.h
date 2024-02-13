#ifndef SIMCAN_EX_DC_RESOURCE_MANAGER
#define SIMCAN_EX_DC_RESOURCE_MANAGER

#include "omnetpp.h"
#include "Messages/VMRequest.h"
#include "OperatingSystem/Hypervisors/DcHypervisor/DcHypervisor.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"

struct Node
{
    enum State
    {
        ACTIVE = 1,    //!< If the hypervisor is powered on
        IN_USE = 2,    //!< If the hypervisor has active users
        MAXED_OUT = 4, //!< If the hypervisor currently cannot allocate any more vms/launch more users
        RESERVED = 8   //!< If the hypervisor is reserved
    };

    NodeResources availableResources; //!< Current available resources
    uint32_t ip;                      //!< Ip of the node, must be unique -- Acts as an id of the Blade (hypervisor)
    uint16_t state;                   //!< The actual state of the vm

    bool isActive() const { return (state & ACTIVE) != 0; }
};

class DcResourceManager : public omnetpp::cSimpleModule
{
protected:
    using CoreHypervisorsMap = std::map<uint32_t, std::vector<uint32_t>>; //!< Pair of available cores / ip

    uint32_t totalCores;        //!< Total cpus in the datacentre
    uint32_t availableCores;    //!< Cpus available in the datacentre
    uint32_t machinesInUse;     //!< Nodes in use
    uint32_t activeMachines;    //!< Active nodes (powered on)
    uint32_t minActiveMachines; //!< Minimum required active nodes

    CoreHypervisorsMap coresHypervisor;
    std::vector<Node> nodes;

    /**
     * @brief Init the map and the super vector keeping the sate of the hypervisors
     * @param stage
     */
    virtual void initialize(int stage) override;
    int numInitStages() const override { return 2; }

    /**
     * @brief Cpu usage times! -> emit signal instad of printing? Global cpu view like the hypervisor?
     * HardwareManager should "expose" the cpuState array!
     */
    virtual void finish() override;

    /**
     * @brief Get the Hypervisor
     * @param nodeIp The node ip where the hypervisor resides
     * @return hypervisor::DcHypervisor* const
     */
    hypervisor::DcHypervisor *const getHypervisor(uint32_t nodeIp);

    void activateNode(uint32_t nodeIp);

    void deactivateNode(uint32_t nodeIp);

public:
    typedef CoreHypervisorsMap::iterator iterator;

    /**
     * @brief Register a node of the dc
     * @param ip Ip of the node
     * @param resources The resources of the node
     * @param isActive If the node is currently active
     */
    void registerNode(uint32_t ip, const NodeResources &resources, bool isActive);

    /**
     * @brief Scans through the nodes and activates or deactivates them to match the objective count
     * @param activeMachines
     */
    void setActiveMachines(uint32_t activeMachines);

    iterator startFromCoreCount(uint32_t coreCount) { return coresHypervisor.lower_bound(coreCount); }
    iterator endOfNodeMap() { return coresHypervisor.end(); }

    // Statistical and management information
    uint32_t getMachinesInUse() const { return machinesInUse; }
    uint32_t getActiveMachines() const { return activeMachines; }
    uint32_t getActiveOrInUseMachines() const { return machinesInUse + activeMachines; }
    uint32_t getTotalCores() const { return totalCores; }
    uint32_t getAvailableCores() const { return availableCores; }
    double getAggregatedCpuUsage() const { return availableCores / (double)totalCores; }

    friend class VmDeployment;
};

/**
 * @brief Class that implements a deployment in a transactional manner
 */
class VmDeployment
{
private:
    struct VmAllocation
    {
        uint32_t nodeIp;
        VM_Request &request;
        NodeResources resources;
    };

    DcResourceManager *resourceManager;
    std::vector<VmAllocation> allocations;

public:
    VmDeployment(DcResourceManager *rm) : resourceManager(rm) {}

    void addNode(const uint32_t &nodeIp, VM_Request &request, const NodeResources &neededResources);

    /**
     * @brief Commits the deployment, making all allocations efective
     */
    void commit();

    /**
     * @brief Rolls back the allocations
     */
    void rollback();

    friend class DcResourceManager;
};

#endif /* SIMCAN_EX_RESOURCE_MANAGER */

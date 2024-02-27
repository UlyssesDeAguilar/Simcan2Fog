#ifndef SIMCAN_EX_DC_RESOURCE_MANAGER
#define SIMCAN_EX_DC_RESOURCE_MANAGER

#include "omnetpp.h"
#include "Messages/SM_UserVM.h"
#include "OperatingSystem/Hypervisors/DcHypervisor/DcHypervisor.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"

class DcResourceManager : public omnetpp::cSimpleModule
{
protected:
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
        size_t bucketIndex;               //!< The position of the node in the current bucket it is in
        uint32_t ip;                      //!< Ip of the node, must be unique -- Acts as an id of the Blade (hypervisor)
        uint16_t state;                   //!< The actual state of the vm

        bool isActive() const { return (state & ACTIVE) != 0; }
        bool inUse() const { return state == (ACTIVE | IN_USE); }
    };

    using CoreHypervisorsMap = std::map<uint32_t, std::vector<uint32_t>>; //!< Pair of available cores / ip

    uint32_t totalCores;         //!< Total cpus in the datacentre
    uint32_t availableCores;     //!< Cpus available in the datacentre
    uint32_t machinesInUse;      //!< Nodes in use
    uint32_t activeMachines;     //!< Active nodes (powered on)
    uint32_t minActiveMachines;  //!< Minimum required active nodes
    uint32_t reservedNodes;      //!< Number of reserved nodes
    GlobalAddress globalAddress; //!< Global L3 address

    CoreHypervisorsMap coresHypervisor;
    CoreHypervisorsMap reservedCoresHypervisor;
    std::vector<Node> nodes;

    virtual void initialize(int stage) override;
    void initBucketsAndReservations();
    virtual int numInitStages() const override { return TOTAL_INIT_STAGES; }
    virtual void finish() override;

    /**
     * @brief Get the Hypervisor
     * @param nodeIp The node ip where the hypervisor resides
     * @return hypervisor::DcHypervisor* const
     */
    hypervisor::DcHypervisor *const getHypervisor(uint32_t nodeIp);

    void activateNode(uint32_t nodeIp);
    void deactivateNode(uint32_t nodeIp);

    void updateNode(uint32_t nodeIp, const VirtualMachine *vm, bool allocate);

    NodeResources &getNodeResources(const uint32_t &ip) { return nodes[ip].availableResources; }

    /**
     * @brief Confirms changes and updates statistics with the given node
     * @details This is called from the VmDeployment class when the Manager decides to commit the deployment.
     *
     * + If the vm has no more essential resources, then it is marked with MAXED_OUT
     * + The global available core count is updated
     *  - Suggestion: This could be perfect point to emit signal statistics!
     *
     * @param ip The node ip where the changes proposed by a deployment are made effective
     * @param vmTemplate The characteristics of the vm allocated inside
     */
    void confirmNodeAllocation(const uint32_t &ip, const VirtualMachine *vmTemplate);

    void confirmNodeDeallocation(const uint32_t &ip, const VirtualMachine *vmTemplate, bool inUse);

public:
    typedef CoreHypervisorsMap::iterator iterator;

    /**
     * @brief Register a node of the dc
     * @param ip Ip of the node
     * @param resources The resources of the node
     */
    void registerNode(uint32_t ip, const NodeResources &resources);

    /**
     * @brief Set the Global Address
     * @param a The global address
     */
    void setGlobalAddress(const GlobalAddress &a) { globalAddress = a; }
    void setMinActiveMachines(uint32_t minimum) { minActiveMachines = minimum; }
    void setReservedNodes(uint32_t nodes) { reservedNodes = nodes; }

    /**
     * @brief Scans through the nodes and activates or deactivates them to match the objective count
     * @param activeMachines
     */
    void setActiveMachines(uint32_t activeMachines);

    iterator startFromCoreCount(uint32_t coreCount, bool reserved = false) { return reserved ? reservedCoresHypervisor.lower_bound(coreCount) : coresHypervisor.lower_bound(coreCount); }
    iterator endOfNodeMap(bool reserved = false) { return reserved ? reservedCoresHypervisor.end() : coresHypervisor.end(); }

    const NodeResources &getNodeResources(const uint32_t &ip) const { return nodes[ip].availableResources; }

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
 * @brief Class that implements a deployment in a transactional mode
 */
class VmDeployment
{
private:
    struct VmAllocation
    {
        uint32_t nodeIp;
        VM_Request *request;
        const VirtualMachine *vm;

        VmAllocation(uint32_t nodeIp,
                     VM_Request *request,
                     const VirtualMachine *vm) : nodeIp(nodeIp),
                                                 request(request),
                                                 vm(vm) {}
    };

    std::vector<VmAllocation> allocations; //!< Historic that keeps the allocations made by the deployment
    DcResourceManager *resourceManager;    //!< The resource manager
    SM_UserVM *vmsRequest;

public:
    VmDeployment(DcResourceManager *rm, SM_UserVM *req) : resourceManager(rm), vmsRequest(req) { allocations.reserve(req->getVmArraySize()); }

    /**
     * @brief Adds a node to the deployment
     *
     * @param nodeIp The node ip
     * @param bucketIndex The bucket index of the node (relating to cores <>--> nodes map)
     * @param request The request chosen to be deployed inside the node
     * @param vm The "template" that contains the needs of the vm
     */
    void addNode(const uint32_t &nodeIp, const size_t &bucketIndex, VM_Request *request, const VirtualMachine *vm);

    /**
     * @brief Rolls back the node allocations
     */
    void rollback();

    /**
     * @brief Commits the deployment, making all allocations efective
     * @details It also:
     *  + Triggers the statistical and management updates of the resource manager
     *  + Calls the corresponding hypervisor module to materialize the "real" management
     *      - Suggestion: This point could be the future insertion of a more realistical kind of deployment.
     */
    void commit();

    friend class DcResourceManager;
};

#endif /* SIMCAN_EX_DC_RESOURCE_MANAGER */

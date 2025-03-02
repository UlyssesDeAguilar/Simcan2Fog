#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "s2f/management/dataClasses/Users/CloudUserInstance.h"
#include "s2f/management/cloudprovider/NodeUpdate_m.h"
#include "s2f/management/managers/ManagerBase.h"
#include "s2f/management/managers/ResourceManager.h"
#include "s2f/management/managers/selection/Strategies.h"
#include "s2f/os/hypervisors/DcHypervisor.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "inet/common/packet/Packet.h"

/**
 * @brief Module that implements a data centre manager
 * @details Works in conjuction with the DcResourceManager to control and deploy services
 * @author (v1) Pablo C. Cañizares
 * @author (v2) Ulysses de Aguilar Gudmundsson
 */
class DcManager : public ManagerBase
{
protected:
    template <class T>
    using StrMap = std::map<std::string, T *>;

    NodeUpdate eventTemplate;

    ResourceManager *resourceManager;
    dc::SelectionStrategy *nodeSelectionStrategy;
    bool showDataCentreConfig;   /** Show information of DataCentres */
    bool forecastActiveMachines; /** Activate forecasting */

    int nCpuStatusInterval;             //!< CpuStatus interval (also used for ManageMachines)
    cMessage *cpuManageMachinesMessage; //!< Manage machines event
    cMessage *cpuStatusMessage;         //!< Cpu status event

    int nActiveMachinesThreshold;
    int nLastMachinesInUseForecasting;

    StrMap<hypervisor::DcHypervisor> acceptedVMsMap; // Map of the accepted VMs
    opp_string_map acceptedUsersRqMap;            // Map of the accepted users (keeps the topic to return messages)
    StrMap<SM_UserAPP> handlingAppsRqMap;            // Map of the accepted applications

    GateInfo networkGates;
    GateInfo localNetworkGates;

    virtual ~DcManager();
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::INITSTAGE_APPLICATION_LAYER + 1; }
    virtual cGate *getOutGate(cMessage *msg) override;

    virtual void initializeSelfHandlers() override;
    virtual void initializeRequestHandlers() override;
    virtual void initializeResponseHandlers() override;

    // Self handlers
    void handleCpuStatus(cMessage *msg);
    void handleManageMachines(cMessage *msg);
    void manageActiveMachines();

    // Request handlers
    void handleVmRequestFits(SIMCAN_Message *sm);
    void handleVmSubscription(SIMCAN_Message *sm);
    void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);
    void handleEndVmAndAbortExecution(SIMCAN_Message *sm);
    
    // Response handlers

    /**
     * @brief Intercepts the timeout emmited by a DcHypervisor, 
     * marks the vm as suspended and sends the user a notification of the timeout
     * @details From this point the user has two options: Extend or Release the vm
     * 
     * @param sm The timeout message (expected to be a SM_UserAPP)
     */
    void handleVmRentTimeout(SIMCAN_Message *sm);

    // Helpers
    bool checkVmUserFit(SM_UserVM *&userVM_Rq);

    // Helpers
    hypervisor::DcHypervisor *getNodeHypervisorByVm(const std::string &vmId) { return acceptedVMsMap.at(vmId); }
    SM_UserAPP *getUserAppRequestPerUser(const std::string &userId) { return handlingAppsRqMap.at(userId); }
    
    /**
     * Calculates the statistics of the experiment.
     */
    virtual void finish() override;

    friend class dc::SelectionStrategy;
    friend class dc::FirstFit;
    friend class dc::BestFit;
    //friend class dc::CostFit;
};

#endif

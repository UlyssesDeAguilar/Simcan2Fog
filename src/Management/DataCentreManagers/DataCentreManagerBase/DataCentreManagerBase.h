#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "Applications/Builder/include.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/Users/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCentreConfigParser.h"
#include "OperatingSystem/Hypervisors/DcHypervisor/DcHypervisor.h"
#include "Management/DataCentreManagers/Selection/Strategies.h"
#include "Management/DataCentreManagers/Selection/Strategies.h"

class DataCentreApplicationBuilder;

/**
 * @brief Module that implements a data centre manager
 * @details Works in conjuction with the DcResourceManager to control and deploy services
 * @author (v1) Pablo C. Cañizares
 * @author (v2) Ulysses de Aguilar Gudmundsson
 */
class DataCentreManagerBase : public CloudManagerBase
{

public:
    int getNTotalAvailableCores() const { return resourceManager->getAvailableCores(); }
    int getNTotalCores() const { return resourceManager->getTotalCores(); }

protected:
    template <class T>
    using StrMap = std::map<std::string, T *>;

    DataCentreApplicationBuilder *appBuilder;
    DcResourceManager *resourceManager;
    dc::SelectionStrategy *nodeSelectionStrategy;

    bool showDataCentreConfig;   /** Show information of DataCentres */
    bool forecastActiveMachines; /** Activate forecasting */

    int nCpuStatusInterval;             //!< CpuStatus interval (also used for ManageMachines)
    cMessage *cpuManageMachinesMessage; //!< Manage machines event
    cMessage *cpuStatusMessage;         //!< Cpu status event

    int nActiveMachinesThreshold;
    int nLastMachinesInUseForecasting;

    StrMap<hypervisor::DcHypervisor> acceptedVMsMap; // Map of the accepted VMs
    StrMap<SM_UserVM> acceptedUsersRqMap;            // Map of the accepted users
    StrMap<SM_UserAPP> handlingAppsRqMap;            // Map of the accepted applications

    GateInfo networkGates;
    GateInfo localNetworkGates;

    /** Vector that contains a collection of structures for monitoring data-centres */
    std::vector<DataCentre *> dataCentresBase;

    std::map<std::string, cModule *> mapAppsVectorModulePerVm;
    std::map<std::string, unsigned int *> mapAppsModulePerId;

    virtual ~DataCentreManagerBase();
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::InitStages::INITSTAGE_APPLICATION_LAYER + 1; }
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
    hypervisor::DcHypervisor *getNodeHypervisorByVm(const std::string &vmId) { return getOrNull(acceptedVMsMap, vmId); }
    unsigned int *getAppModuleById(const std::string &appInstance) { return getOrNull(mapAppsModulePerId, appInstance); }
    SM_UserAPP *getUserAppRequestPerUser(const std::string &userId) { return getOrNull(handlingAppsRqMap, userId); }
    // virtual void deallocateVmResources(std::string strVmId);
    
    /**
     * Calculates the statistics of the experiment.
     */
    virtual void finish() override;
    virtual void printFinal();

    friend class DataCentreApplicationBuilder;
    friend class dc::SelectionStrategy;
    friend class dc::FirstFit;
    friend class dc::BestFit;
    friend class dc::CostFit;
};

#endif

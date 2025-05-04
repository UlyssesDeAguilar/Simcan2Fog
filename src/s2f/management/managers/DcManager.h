#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "s2f/management/dataClasses/Users/CloudUserInstance.h"
#include "s2f/management/cloudprovider/NodeUpdate_m.h"
#include "s2f/management/managers/ManagerBase.h"
#include "s2f/management/managers/ResourceManager.h"
#include "s2f/os/hypervisors/DcHypervisor.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/architecture/net/db/SimpleNetDb.h"
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
    SimpleNetDb *netDb;
    const char *zone;
    const char *nodeName;
    const char *domain;
    bool showDataCentreConfig;   /** Show information of DataCentres */
    bool forecastActiveMachines; /** Activate forecasting */

    int nCpuStatusInterval;             //!< CpuStatus interval (also used for ManageMachines)
    cMessage *cpuManageMachinesMessage; //!< Manage machines event
    cMessage *cpuStatusMessage;         //!< Cpu status event

    int nActiveMachinesThreshold;
    int nLastMachinesInUseForecasting;

    StrMap<hypervisor::DcHypervisor> acceptedVMsMap; // Map of the accepted VMs
    opp_string_map acceptedUsersRqMap;            // Map of the accepted users (keeps the topic to return messages)

    GateInfo networkGates;
    GateInfo queueGates;

    virtual int numInitStages() const override { return inet::INITSTAGE_APPLICATION_LAYER + 1; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    virtual cGate *getOutGate(cMessage *msg) override;
    virtual void initializeSelfHandlers() override;
    virtual void initializeRequestHandlers() override;
    virtual void initializeResponseHandlers() override;
    virtual void handleMessage(cMessage *msg) override;
    
    // Self handlers
    void handleCpuStatus(cMessage *msg);
    void handleManageMachines(cMessage *msg);
    void manageActiveMachines();

    // Request handlers
    void handleVmRequestFits(SIMCAN_Message *sm);
    void handleVmSubscription(SIMCAN_Message *sm);
    void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);
    void handleEndVmAndAbortExecution(SIMCAN_Message *sm);
    void handleApplicationRequest(SIMCAN_Message *sm);
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
    void dispatchVm(const ResourceManager::NodeVmRecord &record, SM_UserVM *request);
    void dispatchVms(const std::vector<ResourceManager::NodeVmRecord> *allocRecords, SM_UserVM *request);
    void sendUpdateToCloudProvider();
    void sendToLocalNetwork(cMessage *msg, int destinationAddress);
    int resolveHypervisorUrl(const char *hypervisorUrl);
    opp_string generateHypervisorUrl(const char *hostName);
};

#endif

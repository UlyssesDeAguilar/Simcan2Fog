#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "Applications/Builder/include.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/Users/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCentreConfigParser.h"
#include "OperatingSystem/Hypervisors/DcHypervisor/DcHypervisor.h"

class DataCentreApplicationBuilder;

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerBase : public CloudManagerBase
{

public:
    int getNTotalAvailableCores() const { return resourceManager->getAvailableCores(); }
    int getNTotalCores() const { return resourceManager->getTotalCores(); }
    void handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndex);

protected:
    template <class T>
    using StrMap = std::map<std::string, T *>;

    DataCentreApplicationBuilder *appBuilder;
    DcResourceManager *resourceManager;

    bool showDataCentreConfig;   /** Show information of DataCentres */
    bool forecastActiveMachines; /** Activate forecasting */

    int nCpuStatusInterval;             //!< CpuStatus interval (also used for ManageMachines)
    cMessage *cpuManageMachinesMessage; //!< Manage machines event
    cMessage *cpuStatusMessage;         //!< Cpu status event

    int nActiveMachinesThreshold;
    int nMinActiveMachines;
    int nLastMachinesInUseForecasting;

    StrMap<hypervisor::DcHypervisor> acceptedVMsMap; // Map of the accepted VMs
    StrMap<SM_UserVM> acceptedUsersRqMap;            // Map of the accepted users
    StrMap<SM_UserAPP> handlingAppsRqMap;            // Map of the accepted applications

    int localInGateId;
    int networkInGateId;

    /** Vector that contains a collection of structures for monitoring data-centres */
    std::vector<DataCentre *> dataCentresBase;

    std::map<std::string, cModule *> mapAppsVectorModulePerVm;
    std::map<std::string, unsigned int *> mapAppsModulePerId;
    std::map<std::string, bool *> mapAppsRunningInVectorModulePerVm;

    ~DataCentreManagerBase();
    virtual void initialize() override;
    virtual cGate *getOutGate(cMessage *msg) override;

    virtual void initializeSelfHandlers() override;
    virtual void initializeRequestHandlers() override;
    virtual void initializeResponseHandlers() override{}; // This module currently doesn't accept responses!

    // Cuando terminemos el resouce manager -> a tomar por el culo
    virtual int parseDataCentreConfig();

    // Handlers muy importantes!
    virtual void handleVmRequestFits(SIMCAN_Message *sm);
    virtual void handleExecVmRentTimeout(cMessage *msg);
    bool checkVmUserFit(SM_UserVM *&userVM_Rq);
    virtual hypervisor::DcHypervisor *selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest) = 0;
    void handleUserAppRequest(SIMCAN_Message *sm);

    /**
     * Accept the user request.
     * @param userVM_Rq VMs User request.
     */
    void acceptVmRequest(SM_UserVM *userVM_Rq);

    /**
     * Rejects the user request.
     * @param userVM_Rq User request.
     */
    void rejectVmRequest(SM_UserVM *userVM_Rq);

    void sendAppResponse(bool accepted, SM_UserAPP *request, std::string *vmId);

    /**
     * Allocate resources in a machine
     * @param pResourceRequest Resources requested
     * @param pHypervisor Pointer to the machine hypervisor
     * @return True if VM is allocated. Else otherwise.
     */
    bool allocateVM(const VM_Request &vm, hypervisor::DcHypervisor *pHypervisor);

    void clearVMReq(SM_UserVM *&userVM_Rq, int lastId);

    // Helpers
    cModule *getFreeAppModuleInVector(const std::string &vmId);
    cModule *getAppsVectorModulePerVm(const std::string &vmId) { return getOrNull(mapAppsVectorModulePerVm, vmId); }
    hypervisor::DcHypervisor *getNodeHypervisorByVm(const std::string &vmId) { return getOrNull(acceptedVMsMap, vmId); }
    // bool *getAppsRunningInVectorModuleByVm(const std::string &vmId) { return getOrNull(mapAppsRunningInVectorModulePerVm, vmId); }
    unsigned int *getAppModuleById(const std::string &appInstance) { return getOrNull(mapAppsModulePerId, appInstance); }
    SM_UserAPP *getUserAppRequestPerUser(const std::string &userId) { return getOrNull(handlingAppsRqMap, userId); }
    // void createDummyAppInAppModule(cModule *pVmAppModule);
    // void cleanAppVectorModule(cModule *pVmAppVectorModule);
    // void abortAllApps(std::string strVmId);
    virtual void deallocateVmResources(std::string strVmId);

    /**
     * Sends a timeout of all VM renting
     * @param userAPP_Rq apps User submission.
     */
    // void timeoutAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);
    // void checkAllAppsFinished(SM_UserAPP *pUserApp, std::string strVmId);
    void rejectVmSubscribe(SM_UserVM *userVM_Rq);
    void notifySubscription(SM_UserVM *userVM_Rq);
    void handleVmSubscription(SIMCAN_Message *sm);

    void handleCpuStatus(cMessage *msg);
    void handleManageMachines(cMessage *msg);
    void manageActiveMachines();

    /**
     * Calculates the statistics of the experiment.
     */
    virtual void finish() override;
    virtual void printFinal();

    friend class DataCentreApplicationBuilder;
};

#endif

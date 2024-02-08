#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "Applications/Builder/include.h"
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
    int getNTotalAvailableCores() const { return nTotalAvailableCores; }
    void setNTotalAvailableCores(int nTotalAvailableCores) { this->nTotalAvailableCores = nTotalAvailableCores; }
    int getNTotalCores() const { return nTotalCores; }
    void setNTotalCores(int nTotalCores) { this->nTotalCores = nTotalCores; }
    void handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndex);

protected:
    template <class T>
    using StrMap = std::map<std::string, T *>;

    /** Show information of DataCentres */
    DataCentreApplicationBuilder *appBuilder;
    bool showDataCentreConfig;
    bool forecastActiveMachines;

    int nCpuStatusInterval;
    int nActiveMachinesThreshold;
    int nMinActiveMachines;
    int nLastMachinesInUseForecasting;

    int nTotalCores;
    int nTotalAvailableCores;
    int nTotalMachines;
    int nTotalActiveMachines;

    cMessage *cpuManageMachinesMessage;
    cMessage *cpuStatusMessage;

    StrMap<hypervisor::DcHypervisor> acceptedVMsMap;    // Map of the accepted VMs
    StrMap<SM_UserVM> acceptedUsersRqMap; // Map of the accepted users
    StrMap<SM_UserAPP> handlingAppsRqMap; // Map of the accepted applications

    /** Users */
    std::vector<CloudUserInstance *> users;

    int localInGateId;
    int networkInGateId;

    /** Vector that contains a collection of structures for monitoring data-centres */
    std::vector<DataCentre *> dataCentresBase;

    std::map<int, std::vector<hypervisor::DcHypervisor *>> mapHypervisorPerNodes;
    std::map<std::string, cModule *> mapAppsVectorModulePerVm;
    std::map<std::string, unsigned int *> mapAppsModulePerId;
    std::map<std::string, bool *> mapAppsRunningInVectorModulePerVm;
    std::map<std::string, std::tuple<unsigned int, simtime_t **, simtime_t *>> mapCpuUtilizationTimePerHypervisor;

    ~DataCentreManagerBase();
    virtual void initialize() override;
    virtual cGate *getOutGate(cMessage *msg) override;

    virtual void initializeSelfHandlers() override;
    virtual void initializeRequestHandlers() override;
    virtual void initializeResponseHandlers() override{}; // This module currently doesn't accept responses!

    virtual int initDataCentreMetadata();
    /**
     * Parsea el parametro con la lista de aplicaciones al vector de aplicaciones
     */
    virtual int parseDataCentreConfig();

    /**
     * String que contiene todas los data centres
     */
    std::string dataCentreToString();

    virtual int storeRackMetadata(cModule *pRackModule);
    virtual int storeNodeMetadata(cModule *pNodeModule);

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

    /**
     * Accepts the app request.
     * @param userAPP_Rq apps User submission.
     * @param strVmId The VM that has finished.
     */
    void acceptAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);

    /**
     * Rejects the user application request.
     * @param userAPP_Rq User app submission.
     */
    void rejectAppRequest(SM_UserAPP *userAPP_Rq);

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
    bool *getAppsRunningInVectorModuleByVm(const std::string &vmId) { return getOrNull(mapAppsRunningInVectorModulePerVm, vmId); }
    unsigned int *getAppModuleById(const std::string &appInstance) { return getOrNull(mapAppsModulePerId, appInstance); }
    SM_UserAPP *getUserAppRequestPerUser(const std::string &userId) { return getOrNull(handlingAppsRqMap, userId); }
    void createDummyAppInAppModule(cModule *pVmAppModule);
    void cleanAppVectorModule(cModule *pVmAppVectorModule);
    void abortAllApps(std::string strVmId);
    virtual void deallocateVmResources(std::string strVmId);

    /**
     * Sends a timeout of all VM renting
     * @param userAPP_Rq apps User submission.
     */
    void timeoutAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);
    void endSingleAppResponse(SM_UserAPP *userAPP_Rq, std::string strVmId, std::string strAppName);
    void checkAllAppsFinished(SM_UserAPP *pUserApp, std::string strVmId);
    void rejectVmSubscribe(SM_UserVM *userVM_Rq);
    void notifySubscription(SM_UserVM *userVM_Rq);
    void handleVmSubscription(SIMCAN_Message *sm);
    void storeAppFromModule(cModule *pVmAppModule);
    void handleCpuStatus(cMessage *msg);
    void handleManageMachines(cMessage *msg);
    void manageActiveMachines();

    void updateCpuUtilizationTimeForHypervisor(hypervisor::DcHypervisor *pHypervisor);
    std::tuple<unsigned int, simtime_t **, simtime_t *> getTimersTupleByHypervisorPath(const std::string &fullPath);
    unsigned int getNumCoresByHypervisorPath(const std::string &fullPath) { return std::get<0>(getTimersTupleByHypervisorPath(fullPath)); }
    simtime_t *getTimerArrayByHypervisorPath(std::string strHypervisorFullPath);
    simtime_t **getStartTimeByHypervisorPath(std::string strHypervisorFullPath);
    double getCurrentCpuPercentageUsage();
    int getMachinesInUse();
    int getActiveOrInUseMachines();
    int getActiveMachines();
    void setActiveMachines(int nNewActiveMachines);

    /**
     * Calculates the statistics of the experiment.
     */
    virtual void finish() override;
    virtual void printFinal();
    
    friend class DataCentreApplicationBuilder;
};

#endif

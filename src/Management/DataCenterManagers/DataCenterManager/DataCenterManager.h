#ifndef __SIMCAN_2_0_DATACENTERMANAGER_H_
#define __SIMCAN_2_0_DATACENTERMANAGER_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCenterConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
//#include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-center manager for cloud environments
 */
class DataCenterManager: public CloudManagerBase{
public:
    int getNTotalAvailableCores() const;
    void setNTotalAvailableCores(int nTotalAvailableCores);
    int getNTotalCores() const;
    void setNTotalCores(int nTotalCores);

public:
    void handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndex);

    protected:

        /** Show information of DataCenters */
        bool showDataCenterConfig;

        int nCpuStatusInterval;

        int nTotalCores;

        int nTotalAvailableCores;

        cMessage *cpuStatusMessage;

        std::map<std::string, Hypervisor*> acceptedVMsMap;
        std::map<std::string, SM_UserVM*> acceptedUsersRqMap;
        std::map<std::string, SM_UserAPP*> handlingAppsRqMap;

        /** Users */
        std::vector<CloudUserInstance*> users;

        /** Input gate from DataCenter. */
        cGate* inGate;

        /** Output gates to DataCenter. */
        cGate* outGate;

        /** Vector that contains a collection of structures for monitoring data-centers */
        std::vector<DataCenter*> dataCentersBase;

        std::map<int, std::vector<Hypervisor*>> mapHypervisorPerNodes;
        std::map<std::string, cModule*> mapAppsVectorModulePerVm;
        std::map<std::string, unsigned int*> mapAppsModulePerId;
        std::map<std::string, bool*> mapAppsRunningInVectorModulePerVm;
        std::map<std::string, std::tuple<unsigned int, simtime_t**, simtime_t*>> mapCpuUtilizationTimePerHypervisor;


        ~DataCenterManager();
        virtual void initialize();

        virtual int initDataCenterMetadata();


        virtual void initializeSelfHandlers() override;
        virtual void initializeRequestHandlers() override;
        void processResponseMessage (SIMCAN_Message *sm) override;
       /**
        * Parsea el parametro con la lista de aplicaciones al vector de aplicaciones
        */
        virtual int parseDataCenterConfig();

       /**
        * String que contiene todas los data centers
        */
        std::string dataCenterToString();

       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg);

        virtual int storeNodeMetadata(cModule *pNodeModule);

        virtual void parseConfig() override;

        virtual void handleVmRequestFits(SIMCAN_Message *sm);
        virtual void handleExecVmRentTimeout(cMessage *msg);

        bool checkVmUserFit(SM_UserVM*& userVM_Rq);

        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest);

        //TODO: refactorizar. esta duplicado en provider.
        int calculateTotalCoresRequested(SM_UserVM* userVM_Rq);
        int getTotalCoresByVmType(std::string strVmType);
        NodeResourceRequest* generateNode(std::string strUserName, VM_Request vmRequest);
        void  fillVmFeatures(std::string strVmType, NodeResourceRequest*& pNode);


        Application* searchAppPerType(std::string strAppType);
        void handleUserAppRequest(SIMCAN_Message *sm);
        void  acceptVmRequest(SM_UserVM* userVM_Rq);
        void  rejectVmRequest(SM_UserVM* userVM_Rq);

        SM_UserVM_Finish* scheduleRentingTimeout (std::string name, std::string strUserName, std::string strVmId, double rentTime);
        void clearVMReq (SM_UserVM*& userVM_Rq, int lastId);

        //Helpers
        cModule* getAppsVectorModulePerVm(std::string strVmId);
        void deleteAppFromModule(cModule *pVmAppModule);
        cModule* getFreeAppModuleInVector(std::string strVmId);
        Hypervisor* getNodeHypervisorByVm(std::string strVmId);
        bool* getAppsRunningInVectorModuleByVm(std::string strVmId);
        unsigned int* getAppModuleById(std::string appInstance);
        void createDummyAppInAppModule(cModule *pVmAppModule);
        void cleanAppVectorModule(cModule *pVmAppVectorModule);
        void abortAllApps(std::string strVmId);
        void deallocateVmResources(std::string strVmId);
        SM_UserAPP* getUserAppRequestPerUser(std::string strUserId);
        void  acceptAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId);
        void  timeoutAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId);
        void  endSingleAppResponse(SM_UserAPP* userAPP_Rq, std::string strVmId, std::string strAppName);
        void checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId);
        void rejectVmSubscribe(SM_UserVM* userVM_Rq);
        void notifySubscription(SM_UserVM* userVM_Rq);
        void handleVmSubscription(SIMCAN_Message *sm);
        void storeAppFromModule(cModule *pVmAppModule);
        void handleCpuStatus(cMessage *msg);

        void updateCpuUtilizationTimeForHypervisor(Hypervisor *pHypervisor);
        std::tuple<unsigned int, simtime_t**, simtime_t*> getTimersTupleByHypervisorPath(std::string strHypervisorFullPath);
        simtime_t** getStartTimeByHypervisorPath(std::string strHypervisorFullPath);
        simtime_t* getTimerArrayByHypervisorPath(std::string strHypervisorFullPath);
        unsigned int getNumCoresByHypervisorPath(std::string strHypervisorFullPath);
        double getCurrentCpuPercentageUsage();

        /**
         * Calculates the statistics of the experiment.
         */
        virtual void finish() override;
        virtual void printFinal();
};

#endif

#ifndef __SIMCAN_2_0_DATACENTREMANAGERBASE_H_
#define __SIMCAN_2_0_DATACENTREMANAGERBASE_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/Users/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCentreConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
//#include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerBase: public CloudManagerBase{
public:
    int getNTotalAvailableCores() const;
    void setNTotalAvailableCores(int nTotalAvailableCores);
    int getNTotalCores() const;
    void setNTotalCores(int nTotalCores);

public:
    void handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndex);

    protected:

        /** Show information of DataCentres */
        bool showDataCentreConfig;

        int nCpuStatusInterval;

        int nActiveMachinesThreshold;

        int nMinActiveMachines;

        int nLastMachinesInUseForecasting;

        bool forecastActiveMachines;

        int nTotalCores;

        int nTotalAvailableCores;

        int nTotalMachines;

        int nTotalActiveMachines;

        cMessage *cpuManageMachinesMessage;
        cMessage *cpuStatusMessage;

        /** Map of the accepted VMs*/
        std::map<std::string, Hypervisor*> acceptedVMsMap;

        /** Map of the accepted users*/
        std::map<std::string, SM_UserVM*> acceptedUsersRqMap;

        /** Map of the accepted applications*/
        std::map<std::string, SM_UserAPP*> handlingAppsRqMap;

        /** Users */
        std::vector<CloudUserInstance*> users;

        /** Input gate from DataCentre. */
        cGate* inGate;

        /** Output gates to DataCentre. */
        cGate* outGate;

        /** Vector that contains a collection of structures for monitoring data-centres */
        std::vector<DataCentre*> dataCentresBase;

        std::map<int, std::vector<Hypervisor*>> mapHypervisorPerNodes;
        std::map<std::string, cModule*> mapAppsVectorModulePerVm;
        std::map<std::string, unsigned int*> mapAppsModulePerId;
        std::map<std::string, bool*> mapAppsRunningInVectorModulePerVm;
        std::map<std::string, std::tuple<unsigned int, simtime_t**, simtime_t*>> mapCpuUtilizationTimePerHypervisor;


        ~DataCentreManagerBase();
        virtual void initialize();

        virtual int initDataCentreMetadata();


        virtual void initializeSelfHandlers() override;
        virtual void initializeRequestHandlers() override;
        void processResponseMessage (SIMCAN_Message *sm) override;
       /**
        * Parsea el parametro con la lista de aplicaciones al vector de aplicaciones
        */
        virtual int parseDataCentreConfig();

       /**
        * String que contiene todas los data centres
        */
        std::string dataCentreToString();

       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg);

        virtual int storeRackMetadata(cModule *pRackModule);
        virtual int storeNodeMetadata(cModule *pNodeModule);

        virtual void parseConfig() override;

        virtual void handleVmRequestFits(SIMCAN_Message *sm);
        virtual void handleExecVmRentTimeout(cMessage *msg);

        bool checkVmUserFit(SM_UserVM*& userVM_Rq);

        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest) = 0;

        //TODO: refactorizar. esta duplicado en provider.
        NodeResourceRequest* generateNode(std::string strUserName, VM_Request vmRequest);
        void  fillVmFeatures(std::string strVmType, NodeResourceRequest*& pNode);


        Application* searchAppPerType(std::string strAppType);
        void handleUserAppRequest(SIMCAN_Message *sm);

        /**
         * Accept the user request.
         * @param userVM_Rq VMs User request.
         */
        void  acceptVmRequest(SM_UserVM* userVM_Rq);

        /**
         * Rejects the user request.
         * @param userVM_Rq User request.
         */
        void  rejectVmRequest(SM_UserVM* userVM_Rq);

        /**
         * Accepts the app request.
         * @param userAPP_Rq apps User submission.
         * @param strVmId The VM that has finished.
         */
        void  acceptAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId);

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
        bool allocateVM (NodeResourceRequest *pResourceRequest, Hypervisor *pHypervisor);

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
        virtual void deallocateVmResources(std::string strVmId);
        SM_UserAPP* getUserAppRequestPerUser(std::string strUserId);


        /**
         * Sends a timeout of all VM renting
         * @param userAPP_Rq apps User submission.
         */
        void timeoutAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId);
        void endSingleAppResponse(SM_UserAPP* userAPP_Rq, std::string strVmId, std::string strAppName);
        void checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId);
        void rejectVmSubscribe(SM_UserVM* userVM_Rq);
        void notifySubscription(SM_UserVM* userVM_Rq);
        void handleVmSubscription(SIMCAN_Message *sm);
        void storeAppFromModule(cModule *pVmAppModule);
        void handleCpuStatus(cMessage *msg);
        void handleManageMachines(cMessage *msg);
        void manageActiveMachines();

        void updateCpuUtilizationTimeForHypervisor(Hypervisor *pHypervisor);
        std::tuple<unsigned int, simtime_t**, simtime_t*> getTimersTupleByHypervisorPath(std::string strHypervisorFullPath);
        simtime_t** getStartTimeByHypervisorPath(std::string strHypervisorFullPath);
        simtime_t* getTimerArrayByHypervisorPath(std::string strHypervisorFullPath);
        unsigned int getNumCoresByHypervisorPath(std::string strHypervisorFullPath);
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
};

#endif

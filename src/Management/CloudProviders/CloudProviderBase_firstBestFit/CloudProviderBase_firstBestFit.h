#ifndef __SIMCAN_2_0_CLOUDPROVIDERBASEFIRSTBESTFIT_H_
#define __SIMCAN_2_0_CLOUDPROVIDERBASEFIRSTBESTFIT_H_

#include "Management/CloudProviders/CloudProviderBase/CloudProviderBase.h"
#include "Management/dataClasses/DataCenterInfoCollection.h"
#include "NodeResourceInfo.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_UserAPP_Finish_m.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include <functional>

#define CPU_SPEED       30000
#define SPEED_W_DISK    120
#define SPEED_R_DISK    250

#define INITIAL_STAGE  "INITIAL_STAGE"
#define EXEC_APP_END  "EXEC_APP_END"
#define EXEC_VM_RENT_TIMEOUT "EXEC_VM_RENT_TIMEOUT"
#define EXEC_APP_END_SINGLE "EXEC_APP_END_SINGLE"
#define EXEC_APP_END_SINGLE_TIMEOUT "EXEC_APP_END_SINGLE_TIMEOUT"
#define MANAGE_SUBSCRIBTIONS  "MANAGE_SUBSCRIBTIONS"
#define USER_SUBSCRIPTION_TIMEOUT  "SUBSCRIPTION_TIMEOUT"
#define SIMCAN_MESSAGE "SIMCAN_Message"

/**
 * Class that parses information about the data-centers.
 *
 */
class CloudProviderBase_firstBestFit: public CloudProviderBase {

protected:
    /** Collection of datacenters*/
    IDataCenterCollection *datacenterCollection;

    /** Map of the accepted users*/
    std::map<std::string, SM_UserVM*> acceptedUsersRqMap;

    /** Map of the accepted applications*/
    std::map<std::string, SM_UserAPP*> handlingAppsRqMap;

    /** Map of finished VMs*/
    std::map<std::string, bool> vmFinished;

    /** Queue of the users that are waiting to be handled*/
    std::vector<SM_UserVM*> subscribeQueue;

    /** Flag that indicates if the process has been finished*/
    bool bFinished;

    /** Handler maps */
    std::map<std::string, std::function<void(cMessage*)>> selfHandlers;
    std::map<int, std::function<void(SIMCAN_Message*)>> requestHandlers;

    /** Destructor */
    ~CloudProviderBase_firstBestFit();

    /** Initialize the cloud provider*/
    virtual void initialize();

    /**
     * Initializes the self message handlers.
     */
    virtual void initializeSelfHandlers();

    /**
     * Initializes the request handlers.
     */
    virtual void initializeRequestHandlers();

    /**
     * Process a self message.
     * @param msg Self message.
     */
    virtual void processSelfMessage(cMessage *msg);

    /**
     * Process a request message.
     * @param sm Request message.
     */
    virtual void processRequestMessage(SIMCAN_Message *sm);

    /**
     * Process a response message from a module in the local node.
     * @param sm Response message.
     */
    virtual void processResponseMessage(SIMCAN_Message *sm);

    //################################################################
    //API
    /**
     * Handle a user APP request.
     * @param userAPP_Rq User APP request.
     */
    virtual void handleUserAppRequest(SIMCAN_Message *sm);

    /**
     * Check if the user request fits in the datacenter
     * @param userVM_Rq User request.
     */
    virtual bool checkVmUserFit(SM_UserVM *&userVM_Rq);
    NodeResourceRequest* generateNode(std::string strUserName, VM_Request vmRequest);
    SM_UserVM_Finish* scheduleRentingTimeout (std::string name, std::string strUserName, std::string strVmId, double rentTime);
    SM_UserAPP_Finish* scheduleAppTimeout (std::string name, std::string strUserName, std::string strAppName, std::string strVmId, double totalTime);
    void clearVMReq (SM_UserVM*& userVM_Rq, int lastId);
    void cancelAndDeleteAppFinishMsgs(SM_UserAPP* userApp, std::string strVmId);
    void checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId);

    /**
     * Update the subscription queue. Analyse the queue in order to find timeouts, and accepting the enqueued VM requests.
     */
    virtual void updateSubsQueue();

    /**
     * Free the resources associated to a VM given its identifier.
     * @param strVmId Identifier of the VM.
     */
    virtual void freeVm(std::string strVmId);

    /**
     * Search a virtual machine per type. This method is used to extract the characteristics necessaries in the fillVmFeatures method.
     * @param strVmType
     * @return
     */
    virtual VirtualMachine* searchVmPerType(std::string strVmType);

    /**
     * Stores a user subscription.
     * @param userVM_Rq VM request.
     */
    virtual void storeVmSubscribe(SM_UserVM *userVM_Rq);

    /**
     * Frees all the VMs associated to an specific user.
     * @param strUsername User to be released.
     */
    virtual void freeUserVms(std::string strUsername);

    /**
     * Handles the initial stage of the Cloud Provider
     * @param msg
     */
    virtual void handleInitialStage(cMessage* msg);

    /**
     * Handles the subscriptions and manage the requests queue, selecting the next request to be processed,
     * taking into account the requests timeouts, requirements etc.
     * @param msg
     */
    virtual void handleManageSubscriptions(cMessage *msg);

    /**
     * Handles the finalisation of a single application execution.
     * @param pUserAppFinish Finished application message.
     */
    virtual void handleAppExecEndSingle(cMessage *msg);

    /**
     * Handles the timeout execution of a VM rent.
     * @param pUserVmFinish Finished VM message.
     */
    virtual void handleExecVmRentTimeout(cMessage *msg);

    /**
     * Handles if a VM request fits in the datacenter.
     * @param userVM_Rq Virtual machine request.
     */
    virtual void handleVmRequestFits(SIMCAN_Message *sm);

    /**
     * Handles a VM subscription
     * @param userVM_Rq User VM request
     */
    virtual void handleVmSubscription(SIMCAN_Message *sm);

    /**
     * Handles the finalisation of an application.
     * @param userAPP_Rq User application request.
     */
    //virtual void handleAppExecEnd(cMessage *msg);

    //END - API
    //################################################################

    virtual void initializeDataCenterCollection();

    /**
     * Load the datacenter information
     */
    virtual void loadNodes();

    /**
     * Get the price of a VM given its type.
     * @param strPrice Type of the VM.
     * @return Price of the VM.
     */
    int getPriceByVmType(std::string strPrice);

    /**
     * Fill the VM request features given its type.
     * @param strType VM type.
     * @param pNode Resource request
     */
    void fillVmFeatures(std::string strType, NodeResourceRequest *&pNode);

    /**
     * Insert a new rack in the system.
     * @param nDataCenter Number of datacenter where the rack is inserted.
     * @param nRack Rack number.
     * @param pRackInfo Rack to be inserted.
     */
    void insertRack(int nDataCenter, int nRack, RackInfo *pRackInfo);

    //Auxiliar
    /**
     * Get the total cores of a virtual machine type.
     * @param strVmType Type of the virtual machine.
     * @return Number of cores.
     */
    int getTotalCoresByVmType(std::string strVmType);

    /**
     * Calculate the total cores requested by an specific user.
     * This method is specially useful in order to check if there exist enough space in the datacenter to handle
     * all the requests of the user.
     * @param userVM_Rq User VM request.
     * @return Total number of cores requested by the user.
     */
    int calculateTotalCoresRequested(SM_UserVM *userVM_Rq);

    /**
     * This method is used to calculate the execution time of the applications.
     * Let us remark that it is used in the first stages of the simulation platform, but it will be replaced
     * by a more realistic environment.
     * @param appType
     * @return
     */
    int TEMPORAL_calculateTotalTime(Application *appType);

    //Notifications
    /**
     * Accept the user request.
     * @param userVM_Rq VMs User request.
     */
    void acceptVmRequest(SM_UserVM *userVM_Rq);

    /**
     * Accepts the app request.
     * @param userAPP_Rq apps User submission.
     */
    //void acceptAppRequest(SM_UserAPP *userAPP_Rq);

    /**
     * Accepts the app request.
     * @param userAPP_Rq apps User submission.
     * @param strVmId The VM that has finished.
     */
    void acceptAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);

    /**
     * Sends a timeout to the app request.
     * @param userAPP_Rq User app request
     */
    //void acceptAppRequestWithTimeout(SM_UserAPP *userAPP_Rq);

    /**
     * Sends a timeout of all VM renting
     * @param userAPP_Rq apps User submission.
     */
    //void timeoutAppRequest(SM_UserAPP *userAPP_Rq);

    /**
     * Sends a timeout of a certain VM renting
     * @param userAPP_Rq apps User submission.
     * @param strVmId The VM that caused the timeout.
     */
    void timeoutAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);

    /**
     * Rejects the user request.
     * @param userVM_Rq User request.
     */
    void rejectVmRequest(SM_UserVM *userVM_Rq);

    /**
     * Rejects the user application request.
     * @param userAPP_Rq User app submission.
     */
    void rejectAppRequest(SM_UserAPP *userAPP_Rq);

    /**
     * Sends to a subscribed user a notification message.
     * @param userVM_Rq User VM request.
     */
    void notifySubscription(SM_UserVM *userVM_Rq);

    /**
     * Sends a timeout subscription to an specific user.
     * @param userVM_Rq User VM request.
     */
    void timeoutSubscription(SM_UserVM *userVM_Rq);

    /**
     * Abort all the applications of an specific user allocated in an specific virtual machine.
     * @param userApp User identifier whose applications will be destroyed.
     * @param strVmId Virtual machine identifier.
     */
    virtual void abortAllApps(SM_UserAPP *userApp, std::string strVmId);

    /**
     * Manages a subscription timeout.
     * @param msg Message timeout
     */
    void handleSubscriptionTimeout(cMessage *msg);

    /**
     * Searches a user in the subscription queue.
     * @param strUsername Username to be searched in the queue.
     * @return The index of the user.
     */
    int searchUserInSubQueue(std::string strUsername, std::string strVmId);

    /**
     * Search an application type by using an identifier.
     * @param strAppType Indentifier of the application
     * @return
     */
    Application* searchAppPerType(std::string strAppType);

};

#endif

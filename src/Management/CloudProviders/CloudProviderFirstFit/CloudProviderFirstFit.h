#ifndef __SIMCAN_2_0_CLOUDPROVIDERFIRSTFIT_H_
#define __SIMCAN_2_0_CLOUDPROVIDERFIRSTFIT_H_

#include "../../dataClasses/DataCentreInfoCollection.h"
#include "../CloudProviderFirstFit/NodeResourceInfo.h"
#include "Management/CloudProviders/CloudProviderBase/CloudProviderBase.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_UserAPP_Finish_m.h"
#include "Messages/SM_CloudProvider_Control_m.h"

#define CPU_SPEED       30000
#define SPEED_W_DISK    120
#define SPEED_R_DISK    250

/**
 * Class that parses information about the data-centres.
 *
 */
class CloudProviderFirstFit: public CloudProviderBase {

protected:
    /** Collection of datacentres*/
    IDataCentreCollection *datacentreCollection;

    /** Map of the accepted users*/
    std::map<std::string, SM_UserVM*> acceptedUsersRqMap;

    /** Map of finished VMs*/
    std::map<std::string, bool> vmFinished;

    /** Queue of the users that are waiting to be handled*/
    std::vector<SM_UserVM*> subscribeQueue;

    /** Destructor */
    ~CloudProviderFirstFit();

    virtual void initialize() override;
    virtual void initializeSelfHandlers() override;
    virtual void initializeRequestHandlers() override;
    virtual void initializeResponseHandlers() override;

    /**
     * Process a request message.
     * @param sm Request message.
     */
    virtual void processRequestMessage(SIMCAN_Message *sm) override;

    //################################################################
    //API
    /**
     * Handle a user APP request.
     * @param userAPP_Rq User APP request.
     */
    virtual void handleUserAppRequest(SIMCAN_Message *sm);

    /**
     * Handle an accept response of a user APP request from the data centre.
     * @param sm Response of user APP request.
     */
    virtual void handleResponseAccept(SIMCAN_Message *sm);

    /**
     * Handle a reject response of a user APP request from the data centre.
     * @param sm Response of user APP request.
     */
    virtual void handleResponseReject(SIMCAN_Message *sm);

    /**
     * Handle an accept response of a user APP request from the data centre.
     * @param sm Response of user APP request.
     */
    virtual void handleResponseAppAccept(SIMCAN_Message *sm);

    /**
     * Handle a timeout response of a user APP request from the data centre.
     * @param sm Response of user APP request.
     */
    virtual void handleResponseAppTimeout(SIMCAN_Message *sm);

    /**
     * Check if the user request fits in the datacentre
     * @param userVM_Rq User request.
     */
    virtual bool checkVmUserFit(SM_UserVM *&userVM_Rq);


    // SM_UserAPP_Finish* scheduleAppTimeout (std::string name, std::string strUserName, std::string strAppName, std::string strVmId, double totalTime);
    void clearVMReq (SM_UserVM*& userVM_Rq, int lastId);
    //void cancelAndDeleteAppFinishMsgs(SM_UserAPP* userApp, std::string strVmId);
    //void checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId);

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
    //virtual void handleAppExecEndSingle(cMessage *msg);

    /**
     * Handles the timeout execution of a VM rent.
     * @param pUserVmFinish Finished VM message.
     */
    //virtual void handleExecVmRentTimeout(cMessage *msg);

    /**
     * Handles if a VM request fits in the datacentre.
     * @param userVM_Rq Virtual machine request.
     */
    virtual void handleVmRequestFits(SIMCAN_Message *sm);

    /**
     * Handles a VM subscription
     * @param userVM_Rq User VM request
     */
    virtual void handleVmSubscription(SIMCAN_Message *sm);
    virtual void handleResponseNotifySubcription(SIMCAN_Message* sm);
    virtual void handleResponseRejectSubcription(SIMCAN_Message* sm);

    /**
     * Handles the finalisation of an application.
     * @param userAPP_Rq User application request.
     */
    //virtual void handleAppExecEnd(cMessage *msg);

    //END - API
    //################################################################

    virtual void initializeDataCentreCollection();

    /**
     * Load the datacentre information
     */
    virtual void loadNodes();

    /**
     * Get the price of a VM given its type.
     * @param vmType Type of the VM.
     * @return Price of the VM.
     */
    int getPriceByVmType(const std::string& vmType);

    /**
     * Fill the VM request features given its type.
     * @param strType VM type.
     * @param pNode Resource request
     */
    void fillVmFeatures(std::string strType, NodeResourceRequest *&pNode);

    /**
     * Insert a new rack in the system.
     * @param nDataCentre Number of datacentre where the rack is inserted.
     * @param nRack Rack number.
     * @param pRackInfo Rack to be inserted.
     */
    void insertRack(int nDataCentre, int nRack, RackInfo *pRackInfo);

    //Auxiliar


    //Notifications
    /**
     * Accept the user request.
     * @param userVM_Rq VMs User request.
     */
    //void acceptVmRequest(SM_UserVM *userVM_Rq);

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
    //void acceptAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId);

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
};

#endif

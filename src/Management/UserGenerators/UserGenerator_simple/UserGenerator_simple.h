#ifndef __SIMCAN_2_0_USERGENERATOR_H_
#define __SIMCAN_2_0_USERGENERATOR_H_

#include "Management/UserGenerators/UserGeneratorBase/UserGeneratorBase.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include <algorithm>
#include <random>
#include <functional>

/**
 * Class that implements a User generator for cloud environments.
 *
 */
class UserGenerator_simple: public UserGeneratorBase {

protected:
    typedef std::map<std::string, std::function<void(cMessage*)>> CallbackStrMap;
    typedef std::map<int, std::function<CloudUserInstance*(SIMCAN_Message*)>> CallbackIntMap;
    typedef std::map<std::string, simsignal_t> SignalMap;

    bool bMaxStartTime_t1_active; // Timeouts active
    double offerAcceptanceRate;   // Rate or probability of acceptance

    // Handlers hashMap
    CallbackStrMap selfMessageHandlers;
    CallbackStrMap requestHandlers;
    CallbackIntMap responseHandlers;

    // Signals
    simsignal_t  requestSignal;
    simsignal_t  responseSignal;
    SignalMap executeSignal;
    SignalMap okSignal;
    SignalMap failSignal;
    SignalMap subscribeSignal;
    SignalMap notifySignal;
    SignalMap timeoutSignal;

    std::map<std::string, int> extensionTimeHashMap;

    /**
     * Destructor
     */
    ~UserGenerator_simple();

    /**
     * Initialize method. Invokes the parsing process to allocate the existing cloud users in the corresponding data structures.
     */
    virtual void initialize() override;

    /**
     * Initializes the signals.
     */
    virtual void initializeSignals();

    /**
     * Initializes the self message handlers.
     */
    virtual void initializeSelfHandlers() override;

    /**
     * Initializes the response handlers.
     */
    virtual void initializeResponseHandlers() override;

    void initializeHashMaps();


    virtual void execute(CloudUserInstance *pUserInstance, SM_UserVM *userVm);

    virtual void finishUser(CloudUserInstance *pUserInstance);

    /**
     * Processes a self message.
     *
     * @param msg Received (self) message.
     */
    virtual void processSelfMessage(cMessage *msg) override;


    /**
     * Processes a self message of type WaitToExecute.
     *
     * @param msg Received (WaitToExecute) message.
     */
    virtual void handleWaitToExecuteMessage(cMessage *msg);

    /**
     * Processes a self message of type USER_GEN_MSG.
     *
     * @param msg Received (USER_REQ_GEN_MSG) message.
     */
    virtual void handleUserReqGenMessage(cMessage *msg);

    /**
     * Schedules the next user generation by scheduling an USER_GEN_MSG.
     *
     */
    virtual void scheduleNextReqGenMessage();
    /**
     * Processes a request message.
     *
     * @param sm Incoming message.
     */
    virtual void processRequestMessage(SIMCAN_Message *sm) override;

    /**
     * Processes a response message from an external module.
     *
     * @param sm Incoming message.
     */
    virtual void processResponseMessage(SIMCAN_Message *sm) override;

    //###############################################
    //API
    /**
     * Returns the next cloud user to be processed
     */
    virtual CloudUserInstance* getNextUser();


    /**
     * Builds an App request given a user
     */
    virtual SM_UserAPP* createAppRequest(SM_UserVM *userVm);

    /**
     * Handles the VM accept response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseVmAccept(SIMCAN_Message *msg);

    /**
     * Handles the VM reject response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseVmReject(SIMCAN_Message *msg);

    /**
     * Handles the App accept response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseAppAccept(SIMCAN_Message *msg);

    /**
     * Handles the App reject response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseAppReject(SIMCAN_Message *msg);

    /**
     * Handles the App timeout response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseAppTimeout(SIMCAN_Message *msg);


    virtual CloudUserInstance* handleSubNotify(SIMCAN_Message *msg);


    virtual CloudUserInstance* handleSubTimeout(SIMCAN_Message *msg);

    /**
     * Handles the App response sent from the CloudProvider
     * @param userApp incoming message
     */
    virtual bool hasToSubscribeVm(SM_UserAPP *userApp);

    /**
     * Sends a request of service submission
     */
    void submitService(SM_UserVM *userVm);

    /**
     * Sends a subscribe message to the cloudprovider
     * @param userVm
     */
    void subscribe(SM_UserVM *userVm);

    /**
     * Checks if all the VMs of a user are in finished state
     */
    virtual bool allVmsFinished(std::string strUserId);

    /**
     * Updates the status of a user
     */
    virtual void updateVmUserStatus(std::string strUserId, std::string strVmId, tVmState state);

    /**
     * Updates the state of the apps of a certain user with the info provided by a SM_UserAPP message
     * @param userApp A pointer to the SM_UserAPP message which contains the new information
     */
    void updateUserApp(SM_UserAPP *userApp);


    /**
     *  This is exactly the overloaded < operator
     */
    inline static bool compareArrivalTime(CloudUserInstance *a, CloudUserInstance *b) {
        return a->getInstanceTimes().arrival2Cloud < b->getInstanceTimes().arrival2Cloud;
    }

    virtual void deleteIfEphemeralMessage(SIMCAN_Message *msg);

private:

    /**
     * This method generates a vm request filled with pre-defined data. Designed for initial debugging
     *
     * @return Object that represents a VM request
     */
    SM_UserVM* createFakeVmRequest();

//    /**
//     * Send a VM request to the cloud provider
//     * @param pUserInstance A pointer to the cloud user instance
//     */
//    void sendRequest(CloudUserInstance *pUserInstance);

    /**
     * Recovers a VM given a user name, and sends a subscribe message to the cloudmanager
     * @param userApp
     */
    void recoverVmAndsubscribe(SM_UserAPP *userApp);

    /**
     * Recovers a VM given a user name, and sends a subscribe message with only that vm to the cloudmanager
     * @param userApp A pointer to the related SM_UserAPP message
     * @param strVmId The VMId of the VM to subscribe to
     */
    void recoverVmAndsubscribe(SM_UserAPP *userApp, std::string strVmId);


    /**
     * Generates a Single VM SM_UserVM
     * @param userApp
     */
    SM_UserVM* sendSingleVMSubscriptionMessage(SM_UserVM *userVM_Orig, std::string vmId);

    /**
     *  Prints the final parameters.
     */
    void printFinal();


    /**
     * Checks if all the users have finished.
     * @return
     */
    bool allUsersFinished();

    /**
     * Calculates the statistics of the experiment.
     */
    virtual void finish() override;

    /**
     * Calculates the statistics of the experiment.
     */
    virtual void calculateStatistics();

    /**
     * Cancels and deletes all the messages corresponding with an specific user instance
     * @param pUserInstance User instance
     */
    void cancelAndDeleteMessages(CloudUserInstance *pUserInstance);
};

#endif

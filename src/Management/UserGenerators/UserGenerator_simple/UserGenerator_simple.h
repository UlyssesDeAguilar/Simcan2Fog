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
    //Timeouts active
    bool bMaxStartTime_t1_active;

    //TODO: Delete
    SimTime m_dInitSim;
    int m_nUsersSent;

    // Timeouts
    double maxStartTime_t1;
    double nRentTime_t2;
    double maxSubTime_t3;
    double maxSubscriptionTime_t4;
    double offerAcceptanceRate;

    // Handlers hashMap
    std::map<std::string, std::function<void(cMessage*)>> selfMessageHandlers;
    std::map<std::string, std::function<void(cMessage*)>> requestHandlers;
    std::map<int, std::function<CloudUserInstance*(SIMCAN_Message*)>> responseHandlers;

    // Signals
    simsignal_t  requestSignal;
    simsignal_t  responseSignal;
    std::map<std::string, simsignal_t> executeSignal;
    std::map<std::string, simsignal_t> okSignal;
    std::map<std::string, simsignal_t> failSignal;
    std::map<std::string, simsignal_t> subscribeSignal;
    std::map<std::string, simsignal_t> notifySignal;
    std::map<std::string, simsignal_t> timeoutSignal;

    std::map<std::string, int> extensionTimeHashMap;

    /** Iterators */
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
     * Returns the time the next user will arrive to the cloud
     *
     * @param pUserInstance Pointer to the user instance.
     * @param last Last user arrival time.
     */
    virtual SimTime getNextTime(CloudUserInstance *pUserInstance, SimTime last);

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
     * Shuffles the arrival time of the generated users
     */
    virtual void generateShuffledUsers();

    /**
     * Builds the VM request which corresponds with the provided user instance.
     */
    virtual SM_UserVM* createVmRequest(CloudUserInstance *pUserInstance);

    /**
     * Builds an App request given a user
     */
    virtual SM_UserAPP* createAppRequest(SM_UserVM *userVm);

    /**
     * Handles the VM response received from the CloudProvider
     * @param msg Incoming message
     * @return Pointer to the user instance related to the received message
     */
    virtual CloudUserInstance* handleResponseAccept(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleResponseReject(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleResponseAppAccept(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleResponseAppReject(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleResponseAppTimeout(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleSubNotify(SIMCAN_Message *msg);
    virtual CloudUserInstance* handleSubTimeout(SIMCAN_Message *msg);

    /**
     * Handles the App response sent from the CloudProvider
     * @param userApp incoming message
     */
    //virtual void handleUserAppResponse(SIMCAN_Message *userApp_RAW);
    //virtual void handleAppResAccept(SM_UserAPP *userApp);
    //virtual void handleAppResReject(SM_UserAPP *userApp);
    //virtual void handleAppResTimeout(SM_UserAPP *userApp);

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

    virtual SM_UserVM* createVmMessage();

    inline static bool compareArrivalTime(CloudUserInstance *a, CloudUserInstance *b) {
        return a->getArrival2Cloud() < b->getArrival2Cloud();
    }

    virtual void deleteIfEphemeralMessage(SIMCAN_Message *msg);

private:

    /**
     * This method generates a vm request filled with pre-defined data. Designed for initial debugging
     *
     * @return Object that represents a VM request
     */
    SM_UserVM* createFakeVmRequest();

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
     * Print the results obtained during the first phase of the experiments.
     */
    void printExperiments_phase1();

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

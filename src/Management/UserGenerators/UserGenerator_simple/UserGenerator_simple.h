#ifndef __SIMCAN_2_0_USERGENERATOR_H_
#define __SIMCAN_2_0_USERGENERATOR_H_

#include "Management/UserGenerators/UserGeneratorBase/UserGeneratorBase.h"
#include "UserModel/Cloud/CloudUserModel.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_VmExtend_m.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include <algorithm>
#include <memory>
#include <random>
#include <functional>

/**
 * Class that implements a User generator for cloud environments.
 *
 */
class UserGenerator_simple : public UserGeneratorBase
{

protected:
    friend class BaseUserModel;
    friend class CloudUserModel;

    typedef std::map<std::string, std::function<void(cMessage *)>> CallbackStrMap;
    typedef std::map<int, std::function<CloudUserInstance *(SIMCAN_Message *)>> CallbackIntMap;
    typedef std::map<std::string, simsignal_t> SignalMap;

    bool bMaxStartTime_t1_active; // Timeouts active
    double offerAcceptanceRate;   // Rate or probability of acceptance

    // Handlers hashMap
    CallbackStrMap selfMessageHandlers;
    CallbackStrMap requestHandlers;
    CallbackIntMap responseHandlers;

    // Signals
    simsignal_t requestSignal;
    simsignal_t responseSignal;
    SignalMap executeSignal;
    SignalMap okSignal;
    SignalMap failSignal;
    SignalMap subscribeSignal;
    SignalMap notifySignal;
    SignalMap timeoutSignal;

    // Keeps track of how many times a VM has been extended it's renting time
    std::map<std::string, int> extensionTimeHashMap;

    std::unique_ptr<BaseUserModel> model;

    /**
     * Initialize method. Invokes the parsing process to allocate the existing cloud users in the corresponding data structures.
     */
    virtual void initialize() override;

    /**
     * Initializes the signals.
     */
    virtual void initializeSignals();

    virtual void initializeSelfHandlers() override;
    virtual void initializeResponseHandlers() override{};

    void initializeHashMaps();

    virtual void finishUser(CloudUserInstance *pUserInstance);

    DataManager *getDataManager() { return dataManager; }

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

    // ###############################################
    // API
    /**
     * Returns the next cloud user to be processed
     */
    virtual CloudUserInstance *getNextUser();
    
    /**
     *  This is exactly the overloaded < operator
     */
    inline static bool compareArrivalTime(CloudUserInstance *a, CloudUserInstance *b)
    {
        return a->getInstanceTimes().arrival2Cloud < b->getInstanceTimes().arrival2Cloud;
    }

private:
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
     * Cancels and deletes all the messages corresponding with an specific user instance
     * @param pUserInstance User instance
     */
    void cancelAndDeleteMessages(CloudUserInstance *pUserInstance);

    BaseUserModel *newUserModelInstance() { return new BaseUserModel(*this); }

public:
    UserGenerator_simple() : model(newUserModelInstance()) {}
};

#endif

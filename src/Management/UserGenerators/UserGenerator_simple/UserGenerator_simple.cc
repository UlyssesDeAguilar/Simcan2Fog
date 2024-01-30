#include "UserGenerator_simple.h"
#include "Management/utils/LogUtils.h"

#define USER_REQ_GEN_MSG "USER_REQ_GEN"

Define_Module(UserGenerator_simple);

UserGenerator_simple::~UserGenerator_simple()
{
}

/**
 * This method initializes the structures and methods
 */
void UserGenerator_simple::initialize()
{
    // Init super-class
    UserGeneratorBase::initialize();

    // Signals initialization
    initializeSignals();

    bMaxStartTime_t1_active = false;

    offerAcceptanceRate = par("offerAcceptanceRate");

    EV_INFO << "UserGenerator::initialize - Base initialized" << endl;

    initializeSelfHandlers();
    initializeResponseHandlers();
    initializeHashMaps();

    EV_INFO << "UserGenerator::initialize - End" << endl;
}

/**
 * This method initializes the signals
 */
void UserGenerator_simple::initializeSignals()
{
    requestSignal = registerSignal("request");
    responseSignal = registerSignal("response");
    executeSignal[""] = registerSignal("execute");
    subscribeSignal[""] = registerSignal("subscribe");
    notifySignal[""] = registerSignal("notify");
    timeoutSignal[""] = registerSignal("timeout");

    for (std::vector<CloudUserInstance *>::iterator it = userInstances.begin(); it != userInstances.end(); ++it)
    {
        CloudUserInstance *pUserInstance = *it;
        for (int i = 0; i < pUserInstance->getTotalVMs(); i++)
        {
            std::string vmId = pUserInstance->getNthVm(i)->getVmInstanceId();

            std::string auxExName = "execute_" + vmId;
            simsignal_t auxEx = registerSignal(auxExName.c_str());
            std::string auxSubName = "subscribe_" + vmId;
            simsignal_t auxSub = registerSignal(auxSubName.c_str());
            std::string auxNotName = "notify_" + vmId;
            simsignal_t auxNot = registerSignal(auxNotName.c_str());
            std::string auxTimName = "timeout_" + vmId;
            simsignal_t auxTim = registerSignal(auxTimName.c_str());
            std::string auxOkName = "appOK_" + vmId;
            simsignal_t auxOk = registerSignal(auxOkName.c_str());
            std::string auxFailName = "appTimeout_" + vmId;
            simsignal_t auxFail = registerSignal(auxFailName.c_str());

            cProperty *exTemplate = getProperties()->get("statisticTemplate", "executeSingleTemplate");
            cProperty *subTemplate = getProperties()->get("statisticTemplate", "subscribeSingleTemplate");
            cProperty *notTemplate = getProperties()->get("statisticTemplate", "notifySingleTemplate");
            cProperty *timTemplate = getProperties()->get("statisticTemplate", "timeoutSingleTemplate");
            cProperty *okTemplate = getProperties()->get("statisticTemplate", "appOKTemplate");
            cProperty *failTemplate = getProperties()->get("statisticTemplate", "appTimeoutTemplate");
            getEnvir()->addResultRecorders(this, auxEx, auxExName.c_str(), exTemplate);
            getEnvir()->addResultRecorders(this, auxSub, auxSubName.c_str(), subTemplate);
            getEnvir()->addResultRecorders(this, auxNot, auxNotName.c_str(), notTemplate);
            getEnvir()->addResultRecorders(this, auxTim, auxTimName.c_str(), timTemplate);
            getEnvir()->addResultRecorders(this, auxOk, auxOkName.c_str(), okTemplate);
            getEnvir()->addResultRecorders(this, auxFail, auxFailName.c_str(), failTemplate);

            executeSignal[vmId] = auxEx;
            subscribeSignal[vmId] = auxSub;
            notifySignal[vmId] = auxNot;
            timeoutSignal[vmId] = auxTim;
            okSignal[vmId] = auxOk;
            failSignal[vmId] = auxFail;
        }
    }
}

/**
 * This method initializes the self message handlers
 */
void UserGenerator_simple::initializeSelfHandlers()
{
    selfMessageHandlers[Timer_WaitToExecute] = [this](cMessage *msg)
    { handleWaitToExecuteMessage(msg); };
    selfMessageHandlers[USER_REQ_GEN_MSG] = [this](cMessage *msg)
    { handleUserReqGenMessage(msg); };
}

/**
 * This method initializes the response handlers
 */
void UserGenerator_simple::initializeResponseHandlers()
{
    responseHandlers[SM_VM_Res_Accept] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleResponseVmAccept(msg); };
    responseHandlers[SM_VM_Res_Reject] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleResponseVmReject(msg); };
    responseHandlers[SM_APP_Res_Accept] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleResponseAppAccept(msg); };
    responseHandlers[SM_APP_Res_Reject] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleResponseAppReject(msg); };
    responseHandlers[SM_APP_Res_Timeout] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleResponseAppTimeout(msg); };
    responseHandlers[SM_APP_Sub_Accept] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleSubNotify(msg); };
    responseHandlers[SM_APP_Sub_Timeout] = [this](SIMCAN_Message *msg) -> CloudUserInstance *
    { return handleSubTimeout(msg); };
}

void UserGenerator_simple::initializeHashMaps()
{
    for (const auto &userInstance : userInstances)
    {
        int nVmCollections = userInstance->getNumberVmCollections();
        std::map<std::string, int> vmExtendedTimes;

        for (int j = 0; j < nVmCollections; j++)
        {
            auto pVmCollection = userInstance->getVmCollection(j);

            // For all instances in the VmCollection
            for (const auto &instance : pVmCollection->allInstances())
            {
                std::string strVmId = instance->getVmInstanceId();
                extensionTimeHashMap[strVmId] = 0;
            }
        }
    }
}

/**
 * This method processes the self messages
 * @param msg
 */
void UserGenerator_simple::processSelfMessage(cMessage *msg)
{
    std::map<std::string, std::function<void(cMessage *)>>::iterator it;

    it = selfMessageHandlers.find(msg->getName());

    if (it == selfMessageHandlers.end())
        error("Unknown self message [%s]", msg->getName());
    else
        it->second(msg);

    delete (msg);
}

void UserGenerator_simple::handleWaitToExecuteMessage(cMessage *msg)
{
    // Log (INFO)
    EV_INFO << "Starting execution!!!" << endl;

    // if (!intervalBetweenUsers) {
    std::sort(userInstances.begin(), userInstances.end(), [](CloudUserInstance *cloudUser1, CloudUserInstance *cloudUser2)
              { return *cloudUser1 < *cloudUser2; });
    //}

    scheduleNextReqGenMessage();
}

void UserGenerator_simple::handleUserReqGenMessage(cMessage *msg)
{

    SM_UserVM *userVm;
    CloudUserInstance *pUserInstance;
    SimTime initTime;
    double dInitHour;

    EV_INFO << "processSelfMessage - USER_REQ_GEN_MSG" << endl;
    // Get current user
    pUserInstance = userInstances.at(nUserIndex);
    userVm = pUserInstance->getRequestVmMsg();

    if (userVm != nullptr)
    {
        // Send user to cloud provider
        // emit(requestSignal, pUserInstance->getId());
        sendRequestMessage(userVm, toCloudProviderGate);

        initTime = simTime() - m_dInitSim;
        dInitHour = initTime.dbl() / 3600;

        // TODO: ¿Dejamos este mensaje o lo quitamos?
        EV_FATAL << "#___ini#" << nUserIndex << " "
                 << dInitHour << endl;
    }
    nUserIndex++;

    scheduleNextReqGenMessage();
}

CloudUserInstance *UserGenerator_simple::getNextUser()
{
    CloudUserInstance *pUserInstance;

    pUserInstance = nullptr;

    if (nUserIndex < userInstances.size())
    {
        pUserInstance = userInstances.at(nUserIndex);
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - The next user to be processed is "
                << pUserInstance->getNId() << " [" << nUserIndex
                << " of " << userInstances.size() << "]" << endl;
        // nUserIndex++; updated at sending to ensure all request are sent
    }
    else
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << "r - The requested user index is greater than the collection size. ["
                << nUserIndex << " of " << userInstances.size() << "]"
                << endl;
    }
    return pUserInstance;
}

void UserGenerator_simple::scheduleNextReqGenMessage()
{
    CloudUserInstance *pUserInstance;
    simtime_t nextArrivalTime;

    // Check if there are more users
    if (nUserIndex < userInstances.size())
    {
        pUserInstance = userInstances.at(nUserIndex);

        // Check if next arrival time is in the future
        nextArrivalTime = SimTime(pUserInstance->getInstanceTimes().arrival2Cloud);
        if (nextArrivalTime < simTime())
            error("Vector of user instances is not sorted by arrival time");

        scheduleAt(nextArrivalTime, new cMessage(USER_REQ_GEN_MSG));
    }
}

void UserGenerator_simple::processRequestMessage(SIMCAN_Message *sm)
{
    error("This module cannot process request messages:%s",
          sm->contentsToString(true, false).c_str());
}

void UserGenerator_simple::processResponseMessage(SIMCAN_Message *sm)
{
    CloudUserInstance *pUserInstance;
    std::map<int, std::function<CloudUserInstance *(SIMCAN_Message *)>>::iterator it;

    EV_INFO << "processResponseMessage - Received Response Message" << endl;

    it = responseHandlers.find(sm->getResult());

    if (it == responseHandlers.end())
        EV_INFO << "processResponseMessage - Unhandled response" << endl;
    else
        pUserInstance = it->second(sm);

    if (pUserInstance != nullptr)
    {
        /** FIXME: Uncomment this!!!
        if (allVmsFinished(pUserInstance->getId()))
        {
            EV_INFO << "Set itself finished" << endl;
            finishUser(pUserInstance);

            cancelAndDeleteMessages(pUserInstance);
        }
        */

        // Check if all the users have ended
        if (pUserInstance->isFinished() && allUsersFinished())
            sendRequestMessage(new SM_CloudProvider_Control(), toCloudProviderGate);
    }
}

void UserGenerator_simple::execute(CloudUserInstance *pUserInstance, SM_UserVM *userVm)
{
    emit(executeSignal[userVm->getStrVmId()], pUserInstance->getNId());
    if (strcmp(userVm->getStrVmId(), "") == 0)
        pUserInstance->getInstanceTimesForUpdate().initExec = simTime();
    submitService(userVm);
}

void UserGenerator_simple::finishUser(CloudUserInstance *pUserInstance)
{
    pUserInstance->getInstanceTimesForUpdate().endTime = simTime();
    pUserInstance->setFinished(true);
    nUserInstancesFinished++;
}

CloudUserInstance *UserGenerator_simple::handleResponseVmAccept(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance;
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(userVm_RAW);

    if (userVm != nullptr)
    {
        EV_INFO << __func__ << " - Response message" << endl;

        userVm->printUserVM();

        // Update the status
        updateVmUserStatus(userVm->getUserID(), userVm->getStrVmId(), vmAccepted);

        // Check the response and proceed with the next action
        pUserInstance = userHashMap.at(userVm->getUserID());

        if (pUserInstance != nullptr)
        {
            emit(responseSignal, pUserInstance->getNId());
            pUserInstance->getInstanceTimesForUpdate().initExec = simTime();

            // If the vm-request has been rejected by the cloudprovider
            // we have to subscribe the service
            pUserInstance->setSubscribe(false);
            emit(executeSignal[""], pUserInstance->getNId());
            submitService(userVm);
        }
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code?)");
    }

    return pUserInstance;
}

CloudUserInstance *UserGenerator_simple::handleResponseVmReject(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(userVm_RAW);

    if (userVm != nullptr)
    {
        EV_INFO << __func__ << " - Response message" << endl;

        userVm->printUserVM();

        // Update the status
        updateVmUserStatus(userVm->getUserID(), userVm->getStrVmId(), vmIdle);

        // Check the response and proceed with the next action
        pUserInstance = userHashMap.at(userVm->getUserID());

        if (pUserInstance != nullptr)
        {
            emit(responseSignal, pUserInstance->getNId());
            pUserInstance->getInstanceTimesForUpdate().initExec = simTime();

            // If the vm-request has been rejected by the cloudprovider
            // we have to subscribe the service
            pUserInstance->setSubscribe(true);
            emit(subscribeSignal[""], pUserInstance->getNId());
            subscribe(userVm);
        }
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code?)");
    }

    return pUserInstance;
}

CloudUserInstance *UserGenerator_simple::handleResponseAppAccept(SIMCAN_Message *msg)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    std::string strVmId;

    if (userApp != nullptr)
    {
        strVmId = userApp->getVmId();
        // Print a debug trace ...
        EV_INFO << *userApp << '\n';

        EV_INFO << __func__ << " - Init" << endl;

        updateVmUserStatus(userApp->getUserID(), strVmId, vmFinished);

        pUserInstance = userHashMap.at(userApp->getUserID());
        if (pUserInstance != nullptr)
            emit(okSignal[strVmId], pUserInstance->getNId());

        deleteIfEphemeralMessage(msg);

        EV_INFO << __func__ << " - End" << endl;
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserAPP (wrong operation code?)");
    }

    return pUserInstance;
}

CloudUserInstance *UserGenerator_simple::handleResponseAppReject(SIMCAN_Message *msg)
{
    CloudUserInstance *pUserInstance;
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    string strVmid;

    if (userApp != nullptr)
    {
        strVmid = userApp->getVmId();
        // Print a debug trace ...
        EV_INFO << *userApp << '\n';

        EV_INFO << __func__ << " - Init" << endl;

        // The next step is to send a subscription to the cloudprovider
        // Recover the user instance, and get the VmRequest
        pUserInstance = userHashMap.at(userApp->getUserID());
        if (pUserInstance != nullptr)
        {
            emit(failSignal[strVmid], pUserInstance->getNId());

            if (hasToSubscribeVm(userApp))
            {
                recoverVmAndsubscribe(userApp);
            }
            else
            {
                // End of the protocol, exit!!
                finishUser(pUserInstance);
                pUserInstance->setTimeoutMaxRentTime();
            }
        }

        deleteIfEphemeralMessage(msg);

        EV_INFO << __func__ << " - End" << endl;
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserAPP (wrong operation code?)");
    }

    return pUserInstance;
}

void UserGenerator_simple::updateUserApp(SM_UserAPP *userApp)
{
    CloudUserInstance *pUserInstance = userHashMap.at(userApp->getUserID());
    std::string strVmId = userApp->getVmId();
    SM_UserAPP *full = pUserInstance->getRequestAppMsg();

    full->update(userApp);
}

CloudUserInstance *UserGenerator_simple::handleResponseAppTimeout(SIMCAN_Message *msg)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    std::string strVmId;

    if (userApp != nullptr)
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

        // Print a debug trace ...
        strVmId = userApp->getVmId();
        EV_INFO << *userApp << '\n';

        pUserInstance = userHashMap.at(userApp->getUserID());
        if (pUserInstance != nullptr)
        {
            emit(failSignal[strVmId], pUserInstance->getNId());

            if (strVmId.empty()) // Global timeout
            {
                if (hasToSubscribeVm(userApp))
                    recoverVmAndsubscribe(userApp);
                else
                    updateVmUserStatus(userApp->getUserID(), "", vmFinished);
            }
            else // Individual VM timeout
            {
                // The next step is to send a subscription to the cloudprovide
                // Recover the user instance, and get the VmRequest

                if (hasToSubscribeVm(userApp))
                {
                    recoverVmAndsubscribe(userApp, strVmId);
                    // pUserInstance->setRequestApp(userApp, strVmId);
                    updateUserApp(userApp);
                }
                else
                {
                    updateVmUserStatus(userApp->getUserID(), strVmId, vmFinished);
                }
            }
            deleteIfEphemeralMessage(msg);
        }

        EV_INFO << __func__ << " - End" << endl;
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserAPP (wrong operation code?)");
    }
    // TODO: Mirar cuando eliminar.  delete pUserInstance->getRequestAppMsg();

    return pUserInstance;
}

CloudUserInstance *UserGenerator_simple::handleSubNotify(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance;
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(userVm_RAW);

    string strVmId;

    if (userVm != nullptr)
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

        userVm->printUserVM();

        // Update the status
        strVmId = userVm->getStrVmId();
        updateVmUserStatus(userVm->getUserID(), strVmId, vmAccepted);

        if (!strVmId.empty())
            extensionTimeHashMap.at(strVmId)++;

        pUserInstance = userHashMap.at(userVm->getUserID());
        EV_INFO << "Subscription accepted ...  " << endl;

        if (pUserInstance != nullptr)
        {

            // Subscription time monitoring
            pUserInstance->endSubscription();

            emit(notifySignal[userVm->getStrVmId()], pUserInstance->getNId());
            execute(pUserInstance, userVm);
        }

        deleteIfEphemeralMessage(userVm_RAW);
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code?)");
    }

    return pUserInstance;
}

CloudUserInstance *UserGenerator_simple::handleSubTimeout(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance;
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(userVm_RAW);

    if (userVm != nullptr)
    {
        EV_INFO << __func__ << " - Init" << endl;

        userVm->printUserVM();

        // Update the status
        updateVmUserStatus(userVm->getUserID(), userVm->getStrVmId(), vmFinished);

        pUserInstance = userHashMap.at(userVm->getUserID());

        // End of the party.
        EV_INFO << "VM timeout ... go home" << endl;

        if (pUserInstance != nullptr)
        {
            pUserInstance->endSubscription();
            pUserInstance->setTimeoutMaxSubscribed();
            emit(timeoutSignal[userVm->getStrVmId()], pUserInstance->getNId());
        }

        deleteIfEphemeralMessage(userVm_RAW);
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code?)");
    }

    return pUserInstance;
}

bool UserGenerator_simple::hasToSubscribeVm(SM_UserAPP *userApp)
{
    double dRandom;

    dRandom = ((double)rand() / (RAND_MAX));

    return dRandom <= offerAcceptanceRate;
}

void UserGenerator_simple::cancelAndDeleteMessages(CloudUserInstance *pUserInstance)
{
    SM_UserVM *pVmMessage;
    SM_UserVM *pSubscribeVmMessage;
    SM_UserAPP *pAppMessage;

    pVmMessage = pUserInstance->getRequestVmMsg();
    pAppMessage = pUserInstance->getRequestAppMsg();
    pSubscribeVmMessage = pUserInstance->getSubscribeVmMsg();

    if (pVmMessage)
    {
        cancelAndDelete(pVmMessage);
        pUserInstance->setRequestVmMsg(nullptr);
    }
    if (pAppMessage)
    {
        cancelAndDelete(pAppMessage);
        pUserInstance->setRequestApp(nullptr);
    }
    if (pSubscribeVmMessage)
        cancelAndDelete(pSubscribeVmMessage);
}

void UserGenerator_simple::recoverVmAndsubscribe(SM_UserAPP *userApp, std::string strVmId)
{
    bool bSent = false;
    SM_UserVM *userVM_Rq;
    std::string strUserId;
    CloudUserInstance *pUserInstance;

    strUserId = userApp->getUserID();

    pUserInstance = userHashMap.at(strUserId);
    if (pUserInstance != nullptr)
    {
        pUserInstance->setSubscribe(true);
        userVM_Rq = sendSingleVMSubscriptionMessage(pUserInstance->getRequestVmMsg(), strVmId);
        bSent = true;
    }

    if (bSent == false)
        error("Error sending the subscription message");
}

void UserGenerator_simple::recoverVmAndsubscribe(SM_UserAPP *userApp)
{
    bool bSent = false;
    SM_UserVM *userVM_Rq;
    std::string strUserId;
    CloudUserInstance *pUserInstance;
    std::string strVmId(userApp->getVmId());

    strUserId = userApp->getUserID();

    if (strUserId.size() > 0)
    {
        EV_INFO << __func__
                << " - Subscribing to the cluster with user: "
                << strUserId << endl;
        pUserInstance = userHashMap.at(strUserId);
        if (pUserInstance != nullptr)
        {
            pUserInstance->setSubscribe(true);
            emit(subscribeSignal[strVmId], pUserInstance->getNId());
            if (strVmId.empty())
            {
                // Ya no se suscribe si llega el timeout final, ya lo hace individualmente.
                userVM_Rq = pUserInstance->getRequestVmMsg();
                if (userVM_Rq != nullptr)
                {
                    bSent = true;
                    subscribe(userVM_Rq);
                    //                    userVM_Rq->setIsResponse(false);
                    //                    userVM_Rq->setOperation(SM_VM_Sub);
                    //                    sendRequestMessage (userVM_Rq, toCloudProviderGate);
                }
            }
        }
    }

    if (bSent == false)
        error("Error sending the subscription message");
}

SM_UserVM *UserGenerator_simple::sendSingleVMSubscriptionMessage(SM_UserVM *userVM_Orig, std::string vmId)
{

    auto userVM = userVM_Orig->dup(vmId);

    if (userVM != nullptr)
    {
        // TODO: Mirar si hace falta.
        userVM->setStrVmId(vmId.c_str());
        subscribe(userVM);
        //        userVM->setIsResponse(false);
        //        userVM->setOperation(SM_VM_Sub);
    }

    return userVM;
}

class cCustomNotification : public cObject, noncopyable
{
public:
    SM_UserVM *userVm;
};

bool UserGenerator_simple::allVmsFinished(std::string strUserId)
{
    CloudUserInstance *pUserInstance = userHashMap.at(strUserId);
    bool result = true;

    if (pUserInstance != nullptr)

        result = pUserInstance->allVmsFinished();
    return result;
}

void UserGenerator_simple::updateVmUserStatus(std::string strUserId, std::string strVmId, tVmState stateNew)
{
    try
    {
        // Find the instance
        auto pUserInstance = userHashMap.at(strUserId);

        // Iterate though collections
        for (int i = 0; i < pUserInstance->getNumberVmCollections(); i++)
        {
            auto pVmCollection = pUserInstance->getVmCollection(i);

            for (const auto &instance : pVmCollection->allInstances())
            {
                if (strVmId.empty() || strVmId.compare(instance->getVmInstanceId()) == 0)
                {
                    // FIXME: This check is weird, probably should only use the new state !
                    tVmState stateOld = instance->getState();
                    instance->setState(stateNew);
                    if (stateOld != vmFinished && stateNew == vmFinished)
                        pUserInstance->addFinishedVMs(1);
                    else if (stateOld == vmFinished && stateNew != vmFinished)
                        pUserInstance->addFinishedVMs(-1);
                }
            }
        }
    }
    catch (const std::out_of_range &e)
    {
        // FIXME: Originally ignored -- Should we launch at least a warning ?
    }
}

void UserGenerator_simple::subscribe(SM_UserVM *userVM_Rq)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Sending Subscribe message" << endl;

    try
    {
        // Subscription time monitoring
        auto pUserInstance = userHashMap.at(userVM_Rq->getUserID());
        pUserInstance->startSubscription();

        if (userVM_Rq != nullptr)
        {
            userVM_Rq->setIsResponse(false);
            userVM_Rq->setOperation(SM_VM_Sub);
            userVM_Rq->printUserVM();
            sendRequestMessage(userVM_Rq, toCloudProviderGate);
        }
        else
        {
            EV_INFO << "Error sending Subscribe message" << endl;
        }
    }
    catch (const std::out_of_range &e)
    {
        // FIXME: Originally ignored -- should be do something about it ?
    }
    EV_INFO << "UserGenerator::subscribe - End" << endl;
}

void UserGenerator_simple::submitService(SM_UserVM *userVm)
{
    SM_UserAPP *pAppRq;
    CloudUserInstance *pUserInstance;
    std::string userName(userVm->getUserID());

    // FIXME: This might throw an exception!
    pUserInstance = userHashMap.at(userName);

    // Execute from response or first notify
    if (strcmp(userVm->getStrVmId(), "") == 0)
    {
        pAppRq = pUserInstance->getRequestAppMsg();

        if (pAppRq == nullptr)
            pAppRq = createAppRequest(userVm);
    }
    else
    {
        EV_WARN << "Not first!" << endl;
        try
        {
            pAppRq = pUserInstance->getRequestAppMsg()->dup(userVm->getStrVmId());
        }
        catch (const std::logic_error &e)
        {
            EV_FATAL << "LOGIC ERROR: APP MSG NOT FOUND!" << endl;
            return;
        }
    }

    // If we're here we do have an app request
    pAppRq->setIsResponse(false);
    pAppRq->setOperation(SM_APP_Req);
    sendRequestMessage(pAppRq, toCloudProviderGate);
}

SM_UserAPP *UserGenerator_simple::createAppRequest(SM_UserVM *userVm)
{
    // FIXME: int for time variables ??? Even in the message definition it's a double !
    int nStartRentTime, nPrice, nIndexRes;

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

    if (userVm != nullptr)
    {
        // FIXME: Warn the simulation ?
        return nullptr;
    }
    auto userApp = new SM_UserAPP();
    std::string strUserName = userVm->getUserID();
    userApp->setUserID(strUserName.c_str());

    // FIXME: Might throw an exception !
    auto pUserInstance = userHashMap.at(strUserName);
    pUserInstance->setRequestApp(userApp);

    for (int i = 0; i < userVm->getTotalVmsRequests(); i++)
    {
        auto vmRq = userVm->getVms(i);

        // Send each app to each VM
        for (int k = 0; k < pUserInstance->getNumberVmCollections(); k++)
        {
            int pAppColSize = pUserInstance->getAppCollectionSize(k);

            for (int j = 0; j < pAppColSize; j++)
            {
                auto pAppInstance = pUserInstance->getAppInstance(j);

                auto strAppType = pAppInstance->getAppName();
                auto strAppInstance = pAppInstance->getAppInstanceId();

                nIndexRes = 0;

                if (userVm->hasResponse(i, nIndexRes))
                {
                    auto pRes = userVm->getResponse(i, nIndexRes);

                    if (pRes != nullptr)
                    {
                        auto strIp = pRes->strIp;
                        auto nStartRentTime = pRes->startTime;
                        auto nPrice = pRes->nPrice;
                        auto strVmId = vmRq.strVmId;

                        if (bMaxStartTime_t1_active)
                        {
                            // Check if T2 <T3
                            if (timeoutsTemplate.maxStartTime >= nStartRentTime)
                            {
                                userApp->createNewAppRequest(strAppInstance + strVmId, strAppType, strIp,
                                                             strVmId, nStartRentTime);
                            }
                            else
                            {
                                // The rent time proposed by the server is too high.
                                EV_INFO
                                    << "The maximum start rent time provided by the cloudprovider is greater than the maximum required by the user: "
                                    << timeoutsTemplate.maxStartTime << " < "
                                    << nStartRentTime << endl;
                            }
                        }
                        else
                        {
                            userApp->createNewAppRequest(strAppInstance + strVmId, strAppType, strIp, strVmId,
                                                         nStartRentTime);
                        }
                    }
                }
                else
                {
                    EV_INFO << "WARNING! Invalid response in user: "
                            << userVm->getName() << endl;
                }
            }
        }
    }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;

    return userApp;
}

bool UserGenerator_simple::allUsersFinished()
{
    bool bRet;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Checking if all users have finished" << endl;

    bRet = nUserInstancesFinished >= userInstances.size();

    EV_INFO << __func__ << " - Res " << bRet << " | NFinished: "
            << nUserInstancesFinished << " vs Total: " << userInstances.size() // Antes comparaba con nUserInstancesFinished. No se actualiza en los timeout
            << endl;

    return bRet;
}

void UserGenerator_simple::finish()
{
    printFinal();
}

void UserGenerator_simple::printFinal()
{
    calculateStatistics();
}
void UserGenerator_simple::calculateStatistics()
{
    double dTotalSub, dMeanSub, dWaitTime, dNoWaitUsers, dWaitUsers;
    int nTotalTimeouts, nCollectionNumber, nInstances, nExtendedTime, nAcceptOffer, nUsersAcceptOffer;
    std::vector<CloudUserInstance *> userVector;
    VmInstanceCollection *pVmCollection;
    VmInstance *pVmInstance;

    // Init general statistics
    nTotalTimeouts = nAcceptOffer = nUsersAcceptOffer = 0;
    dTotalSub = dMeanSub = dWaitTime = 0;
    dNoWaitUsers = dWaitUsers = 0;

    // Small lambda util -- FIXME: Should check if stl has something like this aleady
    auto divideIfNotZero = [](double dividend, double divisor) -> double
    { return dividend != 0 ? dividend / divisor : 0; };

    int nIndex = 1;     // Index for printing purposes
    double dMaxSub = 0; // FIXME: Check this!

    for (const auto &userInstance : userInstances)
    {
        bool bUserAcceptOffer = false;
        dMaxSub = divideIfNotZero(userInstance->getRentTimes().maxSubscriptionTime, 3600);

        auto times = userInstance->getInstanceTimes();
        double dInitTime = divideIfNotZero(times.arrival2Cloud.dbl(), 3600);
        double dEndTime = divideIfNotZero(times.endTime.dbl(), 3600);
        double dExecTime = divideIfNotZero(times.initExec.dbl(), 3600);
        double dWaitTime = divideIfNotZero(times.waitTime.dbl(), 3600);

        // Calculate subscription time
        double dSubTime = dExecTime > dInitTime ? dExecTime - dInitTime : 0;

        if (userInstance->isTimeoutSubscribed())
        {
            EV_FATAL << "#___#Timeout " << nIndex << " -1 " << dMaxSub << " " << dWaitTime << endl;
            dTotalSub += dMaxSub;
            nTotalTimeouts++;
        }
        else
        {
            EV_FATAL << "#___#Success " << nIndex << " " << dSubTime << " -1 "
                     << " " << dWaitTime << endl;
            dTotalSub += dSubTime;
        }

        if (userInstance->hasSubscribed())
            dWaitUsers++;
        else
            dNoWaitUsers++;

        nCollectionNumber = userInstance->getNumberVmCollections();
        for (int i = 0; i < nCollectionNumber; i++)
        {
            pVmCollection = userInstance->getVmCollection(i);
            if (pVmCollection != nullptr)
            {

                // For all instances in the VmCollection
                for (const auto instance : pVmCollection->allInstances())
                {
                    auto strVmId = instance->getVmInstanceId();

                    nExtendedTime = extensionTimeHashMap.at(strVmId);
                    nAcceptOffer += nExtendedTime;
                    if (nExtendedTime > 0)
                    {
                        bUserAcceptOffer = true;
                    }
                }
            }
        }

        if (bUserAcceptOffer)
            nUsersAcceptOffer++;

        nIndex++;
    }

    if (userInstances.size() > 0)
    {
        dMeanSub = dTotalSub / userInstances.size();
    }

    // Print the experiments data
    EV_FATAL << "#___3d#" << dMeanSub << endl;
    EV_FATAL << "#___t#" << dMaxSub << " " << dMeanSub << " " << nTotalTimeouts
             << " " << dNoWaitUsers << " " << dWaitUsers << " " << userInstances.size() << " " << nAcceptOffer << " " << nUsersAcceptOffer << endl;
}

SM_UserVM *UserGenerator_simple::createFakeVmRequest()
{
    SM_UserVM *userVm;
    userVm = createVmMessage();

    // Simple
    userVm->setUserID("Pepe-hardcore");

    // Insert the VMs requests
    userVm->createNewVmRequest("large", "1", 1000, 50, 10, 10);
    userVm->createNewVmRequest("large", "2", 1000, 70, 10, 10);
    userVm->createNewVmRequest("large", "3", 1000, 57, 10, 10);
    userVm->createNewVmRequest("small", "4", 1000, 75, 10, 10);
    userVm->createNewVmRequest("small", "5", 1000, 75, 10, 10);

    userVm->setIsResponse(false);
    userVm->setOperation(SM_VM_Req);

    return userVm;
}

void UserGenerator_simple::deleteIfEphemeralMessage(SIMCAN_Message *msg)
{
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(msg);
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    string strVmId;

    if (userVm != nullptr)
        strVmId = userVm->getStrVmId();

    if (userApp != nullptr)
        strVmId = userApp->getVmId();

    if (!strVmId.empty())
        delete msg;
}

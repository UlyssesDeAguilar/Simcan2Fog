#include "../../../management/UserGenerators/UserGenerator_simple/UserGenerator_simple.h"

#include "../../../management/utils/LogUtils.h"

#define USER_REQ_GEN_MSG "USER_REQ_GEN"

Define_Module(UserGenerator_simple);

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

    EV_INFO << "UserGenerator::initialize - Base initialized" << '\n';

    initializeSelfHandlers();
    initializeResponseHandlers();
    initializeHashMaps();

    EV_INFO << "UserGenerator::initialize - End" << '\n';
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

    cProperty *exTemplate = getProperties()->get("statisticTemplate", "executeSingleTemplate");
    cProperty *subTemplate = getProperties()->get("statisticTemplate", "subscribeSingleTemplate");
    cProperty *notTemplate = getProperties()->get("statisticTemplate", "notifySingleTemplate");
    cProperty *timTemplate = getProperties()->get("statisticTemplate", "timeoutSingleTemplate");
    cProperty *okTemplate = getProperties()->get("statisticTemplate", "appOKTemplate");
    cProperty *failTemplate = getProperties()->get("statisticTemplate", "appTimeoutTemplate");

    for (const auto &instance : userInstances)
    {
        for (const auto &vmInstance : instance->getAllVmInstances())
        {
            std::string vmId = vmInstance.getId();

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

void UserGenerator_simple::initializeHashMaps()
{
    for (const auto &userInstance : userInstances)
    {
        for (const auto &vmInstace : userInstance->getAllVmInstances())
        {
            std::string strVmId = vmInstace.getId();
            extensionTimeHashMap[strVmId] = 0;
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
    EV_INFO << "Starting execution!!!" << '\n';

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

    EV_INFO << "processSelfMessage - USER_REQ_GEN_MSG" << '\n';
    // Get current user
    pUserInstance = userInstances.at(nUserIndex);
    userVm = pUserInstance->getRequestVmMsg();

    if (userVm != nullptr)
    {
        // Send user to cloud provider
        // emit(requestSignal, pUserInstance->getId());
        userVm->setDestinationTopic("CloudProvider");
        sendRequestMessage(userVm, toCloudProviderGate);

        initTime = simTime() - m_dInitSim;
        dInitHour = initTime.dbl() / 3600;

        // TODO: ¿Dejamos este mensaje o lo quitamos?
        EV_FATAL << "#___ini#" << nUserIndex << " "
                 << dInitHour << '\n';
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
                << " of " << userInstances.size() << "]" << '\n';
        // nUserIndex++; updated at sending to ensure all request are sent
    }
    else
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << "r - The requested user index is greater than the collection size. ["
                << nUserIndex << " of " << userInstances.size() << "]"
                << '\n';
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
    // For the time being it must be a SM_VmExtend
    auto request = check_and_cast<SM_VmExtend *>(sm);
    CloudUserInstance *userInstance = userHashMap.at(request->getUserId());
    model->handleVmExtendRequest(request, *userInstance);
}

void UserGenerator_simple::processResponseMessage(SIMCAN_Message *sm)
{
    CloudUserInstance *userInstance = nullptr;

    EV_INFO << "processResponseMessage - Received Response Message" << '\n';

    switch (sm->getOperation())
    {
    case SM_VM_Req:
    {
        auto request = check_and_cast<SM_UserVM *>(sm);
        userInstance = userHashMap.at(request->getUserId());
        model->handleResponseVmRequest(request, *userInstance);
        break;
    }
    case SM_VM_Sub:
    {
        auto request = check_and_cast<SM_UserVM *>(sm);
        userInstance = userHashMap.at(request->getUserId());
        model->handleResponseSubscription(request, *userInstance);
        break;
    }
    case SM_APP_Req:
    {
        auto request = check_and_cast<SM_UserAPP *>(sm);
        userInstance = userHashMap.at(request->getUserId());
        model->handleResponseAppRequest(request, *userInstance);
        break;
    }
    default:
        error("Unable to handle incoming response, please check operation codes of the requests!");
        return;
    }

    if (userInstance->isFinished())
    {
        EV_INFO << "Set itself finished" << '\n';
        nUserInstancesFinished++;
        cancelAndDeleteMessages(userInstance);

        // Check if all the users have ended
        if (allUsersFinished())
            endSimulation(); // FIXME: Brute Force
    }
}

void UserGenerator_simple::cancelAndDeleteMessages(CloudUserInstance *pUserInstance)
{
    SM_UserVM *pVmMessage;

    pVmMessage = pUserInstance->getRequestVmMsg();

    if (pVmMessage)
    {
        cancelAndDelete(pVmMessage);
        pUserInstance->setRequestVmMsg(nullptr);
    }
}

bool UserGenerator_simple::allUsersFinished()
{
    bool bRet;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Checking if all users have finished" << '\n';

    bRet = nUserInstancesFinished >= userInstances.size();

    EV_INFO << __func__ << " - Res " << bRet << " | NFinished: "
            << nUserInstancesFinished << " vs Total: " << userInstances.size() // Antes comparaba con nUserInstancesFinished. No se actualiza en los timeout
            << '\n';

    return bRet;
}

void UserGenerator_simple::finish()
{
    double dTotalSub, dMeanSub, dNoWaitUsers, dWaitUsers;
    int nTotalTimeouts, nCollectionNumber, nExtendedTime, nAcceptOffer, nUsersAcceptOffer;
    std::vector<CloudUserInstance *> userVector;

    // Init general statistics
    nTotalTimeouts = nAcceptOffer = nUsersAcceptOffer = 0;
    dTotalSub = dMeanSub = 0;
    dNoWaitUsers = dWaitUsers = 0;

    // Small lambda util -- FIXME: Should check if stl has something like this aleady
    auto divideIfNotZero = [](int64_t dividend, int64_t divisor) -> int64_t
    { return dividend != 0 ? dividend / divisor : 0; };

    int nIndex = 1;     // Index for printing purposes
    double dMaxSub = 0; // FIXME: Check this!

    for (const auto &userInstance : userInstances)
    {
        bool bUserAcceptOffer = false;

        dMaxSub = divideIfNotZero(userInstance->getRentTimes().maxSubscriptionTime, 3600);
        const InstanceTimes & times = userInstance->getInstanceTimes().convertToHours();

        // Calculate subscription time
        uint64_t subTime = times.initExec > times.arrival2Cloud ? times.initExec - times.arrival2Cloud : 0;

        if (userInstance->isTimeoutSubscribed())
        {
            EV_FATAL << "#___#Timeout " << nIndex << " -1 " << dMaxSub << " " << times.waitTime << '\n';
            dTotalSub += dMaxSub;
            nTotalTimeouts++;
        }
        else
        {
            EV_FATAL << "#___#Success " << nIndex << " " << subTime << " -1 "
                     << " " << times.waitTime << '\n';
            dTotalSub += subTime;
        }

        if (userInstance->hasSubscribed())
            dWaitUsers++;
        else
            dNoWaitUsers++;

        for (const auto &vmInstance : userInstance->getAllVmInstances())
        {
            nExtendedTime = extensionTimeHashMap.at(vmInstance.getId());
            nAcceptOffer += nExtendedTime;
            if (nExtendedTime > 0)
            {
                bUserAcceptOffer = true;
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
    EV_FATAL << "#___3d#" << dMeanSub << '\n';
    EV_FATAL << "#___t#" << dMaxSub << " " << dMeanSub << " " << nTotalTimeouts
             << " " << dNoWaitUsers << " " << dWaitUsers << " " << userInstances.size() << " " << nAcceptOffer << " " << nUsersAcceptOffer << '\n';
}

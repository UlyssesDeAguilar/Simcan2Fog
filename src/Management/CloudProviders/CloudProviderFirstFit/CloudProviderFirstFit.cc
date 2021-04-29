#include "CloudProviderFirstFit.h"

Define_Module(CloudProviderFirstFit);

CloudProviderFirstFit::~CloudProviderFirstFit(){
    acceptedUsersRqMap.clear();
}


void CloudProviderFirstFit::initialize(){
    EV_INFO << "CloudProviderFirstFit::initialize - Init" << endl;

    // Init super-class
    CloudProviderBase::initialize();

    // Init data-centre structures
    //Fill the meta-structures created to improve the performance of the cloudprovider
    initializeDataCentreCollection();

    //TODO: Meta-data only
    //loadNodes();

    bFinished = false;
    scheduleAt(SimTime(), new cMessage(INITIAL_STAGE));
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;
}

void CloudProviderFirstFit::initializeDataCentreCollection(){
    datacentreCollection = new DataCentreInfoCollection();
}

/**
 * This method initializes the self message handlers
 */
void CloudProviderFirstFit::initializeSelfHandlers() {
    // ADAA
    selfHandlers[INITIAL_STAGE] = [this](cMessage *msg) -> void { return handleInitialStage(msg); };
    //selfHandlers[EXEC_VM_RENT_TIMEOUT] = [this](cMessage *msg) -> void { return handleExecVmRentTimeout(msg); };
    //selfHandlers[EXEC_APP_END] = [this](cMessage *msg) -> void { return handleAppExecEnd(msg); };
    //selfHandlers[EXEC_APP_END_SINGLE] = [this](cMessage *msg) -> void { return handleAppExecEndSingle(msg); };
    selfHandlers[USER_SUBSCRIPTION_TIMEOUT] = [this](cMessage *msg) -> void { return handleSubscriptionTimeout(msg); };
    selfHandlers[MANAGE_SUBSCRIBTIONS] = [this](cMessage *msg) -> void { return handleManageSubscriptions(msg); };
}

/**
 * This method initializes the request handlers
 */
void CloudProviderFirstFit::initializeRequestHandlers() {
    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void { return handleVmRequestFits(msg); };
    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void { return handleVmSubscription(msg); };
    requestHandlers[SM_APP_Req] = [this](SIMCAN_Message *msg) -> void { return handleUserAppRequest(msg); };
}


void CloudProviderFirstFit::initializeResponseHandlers() {
    responseHandlers[SM_VM_Res_Accept] = [this](SIMCAN_Message *msg) -> void { return handleResponseAccept(msg); };
    responseHandlers[SM_VM_Res_Reject] = [this](SIMCAN_Message *msg) -> void { return handleResponseReject(msg); };
    responseHandlers[SM_APP_Res_Accept] = [this](SIMCAN_Message *msg) -> void { return handleResponseAppAccept(msg); };
    responseHandlers[SM_APP_Res_Timeout] = [this](SIMCAN_Message *msg) -> void { return handleResponseAppTimeout(msg); };
    responseHandlers[SM_APP_Sub_Accept] = [this](SIMCAN_Message *msg) -> void { return handleResponseNotifySubcription(msg); };
    responseHandlers[SM_APP_Sub_Reject] = [this](SIMCAN_Message *msg) -> void { return handleResponseRejectSubcription(msg); };
//    responseHandlers[SM_APP_End_Single] = [this](SIMCAN_Message *msg) -> void { return handleResponseAppEndSingle(msg); };
}

void CloudProviderFirstFit::processRequestMessage (SIMCAN_Message *sm)
{
    SM_CloudProvider_Control* userControl;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << endl;

    userControl = dynamic_cast<SM_CloudProvider_Control *>(sm);

    if(userControl != nullptr)
      {
        sendRequestMessage (userControl->dup(), toDataCentreGates[0]);
      }
      CloudProviderBase::processRequestMessage(sm);

}



void CloudProviderFirstFit::loadNodes()
{
    DataCentre* pDataCentre;
    Rack* pRack;
    RackInfo* pRackInfo;
    int nComputingRacks, nTotalNodes, nDataCentre, nIndex;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;
    //Initialize
    nComputingRacks =  nTotalNodes = nDataCentre = nIndex=0;

    nDataCentre = dataCentresBase.size();

    //Go over the datacentre object: all racks
    if(nDataCentre>0)
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - handling the DC: " <<  dataCentresToString() << endl;
        for(int nIndex=0;nIndex<nDataCentre;nIndex++)
        {
            EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - loading dataCentre: " << nIndex << endl;
            pDataCentre = dataCentresBase.at(nIndex);
            if(pDataCentre != nullptr)
            {
                nComputingRacks = pDataCentre->getNumRacks(false);
                EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Number of computing racks: "<< nComputingRacks << endl;
                for(int i=0;i<nComputingRacks; i++)
                {
                    EV_DEBUG << "Getting computing rack: "<< i << endl;

                    pRack = pDataCentre->getRack(i, false);

                    if(pRack != NULL)
                    {
                        nTotalNodes = pRack->getNumNodes();

                        EV_DEBUG << "Total nodes:" <<  nTotalNodes << endl;
                        if(nTotalNodes >0)
                        {
                            //Insert the Rack
                            pRackInfo = pRack->getRackInfo();
                            insertRack(nIndex, i, pRackInfo);
                        }
                        else
                        {
                            EV_INFO << "WARNING!! The rack is empty :<"<< i << endl;
                        }
                    }
                }
            }
        }
    }

    //Print the content of the loaded DC
    datacentreCollection->printDCSizes();

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Ende" << endl;
}
void CloudProviderFirstFit::abortAllApps(SM_UserAPP* userApp, std::string strVmId)
{
    if(userApp != nullptr)
    {
        userApp->abortAllApps(strVmId);

        cancelAndDeleteAppFinishMsgs(userApp, strVmId);
    }
}

////Todo: tiene una nota interna. Supongo que se refiere a eliminar el mensaje de otras apps que esten en otro estado.
void CloudProviderFirstFit::cancelAndDeleteAppFinishMsgs(SM_UserAPP* userApp, std::string strVmId)
{
    // Terminar este método, para cancelar el resto de eventos si se requiere.
    for (unsigned int nIndex=0; nIndex<userApp->getAppArraySize(); nIndex++)
    {
        APP_Request& userAppReq = userApp->getApp(nIndex);
        if (userAppReq.vmId.compare(strVmId)==0 && userAppReq.eState == appFinishedTimeout && userAppReq.pMsgTimeout!=nullptr)
        {
            cancelAndDelete(userAppReq.pMsgTimeout);
            userAppReq.pMsgTimeout = nullptr;
        }
    }
}

void CloudProviderFirstFit::handleInitialStage(cMessage* msg) { // ADAA
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - INITIAL_STAGE" << endl;
    scheduleAt(simTime() + SimTime(1), new cMessage(MANAGE_SUBSCRIBTIONS));
}

void CloudProviderFirstFit::handleManageSubscriptions(cMessage* msg)
{
    SM_UserVM* userVmSub;
    std::string strUsername;
    bool bFreeSpace;
    double dWaitingSub, dMaxSubtime;


    bFreeSpace = false;
    if (subscribeQueue.size() > 0)
      {
        EV_TRACE << "The queue of subscription: " << subscribeQueue.size()
                                << endl;

        //First of all, check the subscriptions timeouts
        for (int i = 0; i < subscribeQueue.size(); i++)
          {
            userVmSub = subscribeQueue.at(i);

            if (userVmSub != nullptr)
              {
                strUsername = userVmSub->getUserID();

                dWaitingSub = (simTime().dbl())
                                - (userVmSub->getDStartSubscriptionTime());
                dMaxSubtime = userVmSub->getMaxSubscribetime(0);

                if (dWaitingSub > dMaxSubtime)
                  {
                    EV_INFO << "The subscription of the user:  " << strUsername
                            << " has expired, TIMEOUT! SimTime()"
                            << simTime().dbl() << " - "
                            << userVmSub->getDStartSubscriptionTime()
                            << " = | " << dWaitingSub << " vs "
                            << dMaxSubtime << endl;
                    freeUserVms(strUsername);
                    bFreeSpace = true;
                    subscribeQueue.erase(subscribeQueue.begin() + i);
                    i--;

                    userVmSub->setDEndSubscriptionTime(simTime().dbl());

                    //send the Timeout
                    timeoutSubscription(userVmSub);
                  }
                else
                  {
                    EV_TRACE << "The subscription of the user:  " << strUsername
                            << " has time to wait!" << simTime().dbl()
                            << " - "
                            << userVmSub->getDStartSubscriptionTime()
                            << " = | " << dWaitingSub << " vs "
                            << dMaxSubtime << " || "
                            << dWaitingSub - dMaxSubtime << endl;
                  }
              }
          }
        //Check the subscription queue
        updateSubsQueue();
      }
    else
      {
        EV_TRACE << "The subscription queue is empty" << endl;
      }
}

void CloudProviderFirstFit::handleSubscriptionTimeout(cMessage *msg)
{
    int nIndex;
    double dWaitingSub, dMaxSubTime;
    SM_UserVM_Finish *pUserSubFinish;
    SM_UserVM *userVmSub;
    std::string strUsername;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - RECEIVED TIMEOUT " << endl;
    EV_TRACE << __func__ << " - Init" << endl;

    nIndex = 0;
    dWaitingSub = dMaxSubTime = 0;
    userVmSub = nullptr;

    if((pUserSubFinish = dynamic_cast<SM_UserVM_Finish*>(msg)))
      {
        strUsername = pUserSubFinish->getUserID();

        nIndex = searchUserInSubQueue(strUsername, pUserSubFinish->getStrVmId());
        if(nIndex != -1)
          {
            EV_TRACE << __func__ << " - User found at position:" << nIndex << endl;
            userVmSub = subscribeQueue.at(nIndex);
            userVmSub->setTimeoutSubscribeMsg(nullptr);
//            if (strcmp(userVmSub->getStrVmId(), "") == 0)
//                freeUserVms(strUsername);

            dWaitingSub = (simTime().dbl())-(userVmSub->getDStartSubscriptionTime());
            dMaxSubTime = userVmSub->getMaxSubscribetime(0);

            subscribeQueue.erase(subscribeQueue.begin()+nIndex);

            if(abs((int)dWaitingSub - (int)dMaxSubTime)<=1)
              {
                EV_INFO << "The subscription of the user:  " << strUsername << " has expired, TIMEOUT! SimTime()" << simTime().dbl()<< " - " <<
                        userVmSub->getDStartSubscriptionTime() << " = | " << dWaitingSub << " vs " << dMaxSubTime  << endl;

                userVmSub->setDEndSubscriptionTime(simTime().dbl());
                //freeUserVms(strUsername);
                timeoutSubscription(userVmSub);
              }
            else
              {
                EV_TRACE << "The subscription of the user:  " << strUsername << " has time waiting!"
                << simTime().dbl()<< " - " <<
                userVmSub->getDStartSubscriptionTime() << " = | " << dWaitingSub << " vs " << dMaxSubTime  << endl;
              }
            //send the Timeout
          }
        else
          {
            EV_TRACE << __func__ << " - User " << strUsername << " not found in queue list" << endl;
          }
        //Check the subscription queue
        //updateSubsQueue();
      }
    else
      {
        error ("%s - Unable to cast msg to SM_UserVM_Finish*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), msg->getName());
      }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - USER_SUBSCRIPTION_TIMEOUT End" << endl;
}

int CloudProviderFirstFit::searchUserInSubQueue(std::string strUsername, std::string strVmId)
{
    int nRet, nIndex;
    SM_UserVM* pUserVM;
    bool bFound;

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;
    nRet = -1;
    bFound= false;
    nIndex=0;

    EV_TRACE << __func__ << " - Searching for user: " << strUsername << endl;
    if(strUsername.size()>0)
      {
        EV_TRACE << __func__ << " - Queue size: " << subscribeQueue.size() << endl;
        while(!bFound && nIndex < subscribeQueue.size())
          {
            pUserVM = subscribeQueue.at(nIndex);

            if(pUserVM != nullptr)
              {
                if(strUsername.compare(pUserVM->getUserID()) == 0 && (strcmp(pUserVM->getStrVmId(), "") == 0 || strVmId.compare(pUserVM->getStrVmId()) == 0))
                  {
                    bFound = true;
                  } else {
                    EV_TRACE << __func__ << " - [nIndex: " << nIndex << " " << strUsername << " vs " <<pUserVM->getUserID()<<" ]" << endl;
                    nIndex++;
                  }

              }
            else
              {
                EV_INFO << __func__ << " - WARNING! null pointer at position[nIndex: " << nIndex << " ]" << endl;
                nIndex++;
              }
          }
      }

    if(bFound)
        nRet = nIndex;

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;

    return nRet;
}
void CloudProviderFirstFit::updateSubsQueue()
{
    SM_UserVM* userVmSub;

    //Finally, notify if there is enough space to handle a new notification
    for(int i=0;i<subscribeQueue.size();i++)
    {
        userVmSub = subscribeQueue.at(i);

        userVmSub->setOperation(SM_VM_Sub);
        //userVmSub->setResult(0);

        // If the message is not in the datacentre yet
        if (userVmSub->getOwner()==this && userVmSub->getResult()!=SM_APP_Sub_Accept) {
            checkVmUserFit(userVmSub);

            EV_INFO << "Notifying subscription of user: "<< userVmSub->getUserID() << endl;
        }
    }
}

void CloudProviderFirstFit::freeUserVms(std::string strUsername)
{
    std::string strVmId;
    SM_UserVM* pVmRequest;
    std::map<std::string, SM_UserVM*>::iterator it;
    VM_Request vmRequest;

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

    //Mark the user VMs as free
    it = acceptedUsersRqMap.find(strUsername);
    if(it != acceptedUsersRqMap.end())
      {
        pVmRequest = it->second;

        for(int i=0;i<pVmRequest->getVmsArraySize();i++)
          {
            vmRequest = pVmRequest->getVms(i);
            strVmId = vmRequest.strVmId;
            freeVm(strVmId);
          }

        acceptedUsersRqMap.erase(strUsername);
      }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;
}

void CloudProviderFirstFit::freeVm(std::string strVmId)
{
    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;
    if (!strVmId.empty())
      {
        EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Releasing the Vm:  "<< strVmId << endl;

        if(datacentreCollection->freeVmRequest(strVmId))
            EV_DEBUG << "the Vm has been released sucessfully: "<< strVmId << endl;
        else //Error freeing the VM
            EV_INFO << "Error releasing the VM: "<< strVmId << endl;
      }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;
}

void CloudProviderFirstFit::insertRack(int nIndex, int nRack, RackInfo* pRackInfo)
{
    NodeInfo* pNodeInfo;
    NodeResourceInfo* pNodeResInfo;
    int nTotalCpus, nTotalMemory, cpuSpeed, nTotalNodes;
    double totalDiskGB, totalMemoryGB;
    bool storage;

    storage= false;
    nTotalCpus = nTotalMemory = cpuSpeed = nTotalNodes = 0;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init "<< endl;

    if(pRackInfo != NULL)
    {
        pNodeInfo = pRackInfo->getNodeInfo();
        if(pNodeInfo != NULL)
        {
            //Retrieve all the values
            nTotalCpus =  pNodeInfo->getNumCpUs();
            nTotalMemory = pNodeInfo->getTotalMemoryGb();
            cpuSpeed = pNodeInfo->getCpuSpeed();
            totalDiskGB = pNodeInfo->getTotalDiskGb();
            totalMemoryGB = pNodeInfo -> getTotalMemoryGb();
            storage = pNodeInfo->isStorage();
            nTotalNodes =pRackInfo->getNumBoards()*pRackInfo->getNodesPerBoard();

            for(int j=0;j<nTotalNodes;j++)
            {
                EV_TRACE << "Adding computing node : "<< j << " | rack: " << nRack << endl;

                pNodeResInfo = new NodeResourceInfo();
                pNodeResInfo->setNumCpUs(nTotalCpus);
                pNodeResInfo->setAvailableCpUs(nTotalCpus);
                pNodeResInfo->setTotalMemoryGb(nTotalMemory);
                pNodeResInfo->setAvailableMemory(nTotalMemory);
                pNodeResInfo->setCpuSpeed(cpuSpeed);
                pNodeResInfo->setTotalDiskGb(totalDiskGB);
                pNodeResInfo->setAvailableDiskGb(totalDiskGB);

                pNodeResInfo->setIp("dc:"+std::to_string(nIndex)+"_rack:"+std::to_string(nRack)+"_node:"+std::to_string(j));
                pNodeResInfo->setDataCentre(nIndex);

                EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Inserting node "<< j << " at datacentre: " << nIndex << endl;
                //Insert the node info into the corresponding DC
                datacentreCollection->insertNode(nIndex, pNodeResInfo);
            }
        }
    }

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - End "<< endl;
}

void CloudProviderFirstFit::handleVmRequestFits(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request"  << endl;

    if(userVM_Rq != nullptr)
      {
        userVM_Rq->printUserVM();
        //Check if is a VmRequest or a subscribe
        if (subscribeQueue.size()>0)
            rejectVmRequest(userVM_Rq);
        else if (checkVmUserFit(userVM_Rq)) {
            //acceptVmRequest(userVM_Rq);
            EV_FATAL << "OK" << endl;
        } else {
            EV_FATAL << "Fail" << endl;
            rejectVmRequest(userVM_Rq);
        }
      }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());
      }
}

void CloudProviderFirstFit::handleVmSubscription(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Subscribe operation"  << endl;

    if(userVM_Rq != nullptr)
      {
        storeVmSubscribe(userVM_Rq);
        updateSubsQueue();
      }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());
      }
}

void CloudProviderFirstFit::handleUserAppRequest(SIMCAN_Message *sm)
{
    //Get the user name, and recover the info about the VmRequests;
    SM_UserAPP *userAPP_Rq;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle AppRequest"  << endl;
    userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);

    if(userAPP_Rq != nullptr)
      {
            sendRequestMessage (userAPP_Rq, toDataCentreGates[0]);
      }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userAPP_Rq. Nullpointer!!").c_str());
      }
}

Application* CloudProviderFirstFit::searchAppPerType(std::string strAppType)
{
    Application* appTypeRet;
    bool bFound;
    int nIndex;

    appTypeRet = nullptr;
    bFound = false;
    nIndex = 0;

    EV_DEBUG << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

    while(!bFound && nIndex < appTypes.size())
      {
        appTypeRet = appTypes.at(nIndex);
        if(strAppType.compare(appTypeRet->getAppName()) == 0)
            bFound = true;

        EV_DEBUG << __func__ << " - " << strAppType << " vs " << appTypeRet->getAppName() << " Found=" << bFound << endl;

        nIndex++;
      }

    if(!bFound)
        appTypeRet = nullptr;

    return appTypeRet;
}
void CloudProviderFirstFit::handleResponseRejectSubcription(SIMCAN_Message* sm)
{
    SM_UserVM *userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request"  << endl;

    if(userVM_Rq == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

    //storeVmSubscribe(userVM_Rq);
}
void CloudProviderFirstFit::notifySubscription(SM_UserVM* userVM_Rq)
{
    SM_UserVM_Finish* pMsgTimeout;

    EV_INFO << "Notifying request from user: " << userVM_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << endl;

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Accept);

    //Cancel the timeout event
    pMsgTimeout = userVM_Rq->getTimeoutSubscribeMsg();
    userVM_Rq->setTimeoutSubscribeMsg(nullptr);
    cancelAndDelete(pMsgTimeout);


    //Send the values
    sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFit::timeoutSubscription(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << endl;

    userVM_Rq->setTimeoutSubscribeMsg(nullptr);

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Timeout);

    //Send the values
    sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFit::storeVmSubscribe(SM_UserVM* userVM_Rq)
{
    double dMaxSubscribeTime;
    SM_UserVM_Finish *pMsg;
    std::string strUserName;

    if(userVM_Rq != nullptr)
    {
        strUserName = userVM_Rq->getUserID();
        dMaxSubscribeTime = userVM_Rq->getMaxSubscribetime(0);
        EV_INFO << "Subscribing the VM request from user:" << strUserName << " | max sub time: " << dMaxSubscribeTime<<endl;

        pMsg = userVM_Rq->getTimeoutSubscribeMsg();
        if (pMsg==nullptr)
        {
            pMsg = scheduleVmMsgTimeout(USER_SUBSCRIPTION_TIMEOUT, userVM_Rq->getUserID(), userVM_Rq->getStrVmId(), "", dMaxSubscribeTime);

            //Store the VM subscription until there exist the Vms necessaries
            userVM_Rq->setDStartSubscriptionTime(simTime().dbl());
            userVM_Rq->setTimeoutSubscribeMsg(pMsg);
        }

        subscribeQueue.push_back(userVM_Rq);

        //scheduleAt(simTime() + SimTime(dMaxSubscribeTime), pMsg);
    }
}

SM_UserAPP_Finish* CloudProviderFirstFit::scheduleAppTimeout (std::string name, std::string strUserName, std::string strAppName, std::string strVmId, double totalTime)
{
    SM_UserAPP_Finish *pMsgFinish = new SM_UserAPP_Finish();

    pMsgFinish->setUserID(strUserName.c_str());
    pMsgFinish->setStrApp(strAppName.c_str());

    if(!strVmId.empty())
        pMsgFinish ->setStrVmId(strVmId.c_str());

    pMsgFinish->setNTotalTime(totalTime);
    pMsgFinish->setName(name.c_str());

    EV_INFO << "Scheduling time rental Msg, " << strAppName << " at " << simTime().dbl() + totalTime << "s" << endl;
    scheduleAt(simTime() + SimTime(totalTime), pMsgFinish);

    return pMsgFinish;
}

void CloudProviderFirstFit::clearVMReq (SM_UserVM*& userVM_Rq, int lastId)
{
    for(int i = 0; i < lastId ; i++)
      {
        VM_Request& vmRequest = userVM_Rq->getVms(i);
        cancelAndDelete(vmRequest.pMsg);
        vmRequest.pMsg = nullptr;
        datacentreCollection->freeVmRequest(vmRequest.strVmId);
      }
}

bool CloudProviderFirstFit::checkVmUserFit(SM_UserVM*& userVM_Rq)
{
    bool bRet;
    int nTotalAvailableCores, nTotalCoresRequested;

    std::string nodeIp,
                strVmId;

    bRet = false;


    nTotalAvailableCores = dataCentreManagers[0]->getNTotalAvailableCores();
    nTotalCoresRequested = calculateTotalCoresRequested(userVM_Rq);

    if (nTotalAvailableCores >= nTotalCoresRequested) {
        userVM_Rq->setIsResponse(false);

        //TODO: Select datacentre
        sendRequestMessage (userVM_Rq, toDataCentreGates[0]);
        bRet = true;
    }

    EV_DEBUG << "checkVmUserFit- End" << endl;

    return bRet;
}
//
//cGate* CloudProviderFirstFit::selectDataCentre(SM_UserVM*& userVM_Rq) {
//    cGate* toDataCenterGate = nullptr;
//
//    int nTotalRequestedCores,
//        nRequestedVms,
//        nAvailableCores,
//        nTotalCores;
//
//    std::string nodeIp,
//                strUserName,
//                strVmId;
//
//    bAccepted = bRet = true;
//    if(userVM_Rq != nullptr)
//      {
//        nRequestedVms = userVM_Rq->getTotalVmsRequests();
//
//        EV_DEBUG << "checkVmUserFit- Init" << endl;
//        EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userVM_Rq->getUserID() << endl;
//
//        //Before starting the process, it is neccesary to check if the
//        nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
//        nAvailableCores = datacentreCollection->getTotalAvailableCores();
//
//        if(nTotalRequestedCores<=nAvailableCores)
//          {
//            nTotalCores = datacenterCollection->getTotalCores();
//            EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserID() << nTotalRequestedCores<< " vs Available ["<< nAvailableCores << "/" <<nTotalCores << "]"<<endl;
//
//            strUserName = userVM_Rq->getUserID();
//            //Process all the VMs
//            for(int i=0;i<nRequestedVms && bRet;i++)
//              {
//                EV_DEBUG << endl <<"checkVmUserFit - Trying to handle the VM: " << i << endl;
//
//                //Get the VM request
//                VM_Request& vmRequest = userVM_Rq->getVms(i);
//
//                //Create and fill the noderesource  with the VMrequest
//                NodeResourceRequest *pNode = generateNode(strUserName, vmRequest);
//
//                //Send the request to the DC
//                bAccepted = datacentreCollection->handleVmRequest(pNode);
//
//                //We need to know the price of the Node.
//                userVM_Rq->createResponse(i, bAccepted, pNode->getStartTime(), pNode->getIp(), pNode->getPrice());
//                bRet &= bAccepted;
//
//                if(!bRet)
//                  {
//                    clearVMReq (userVM_Rq, i);
//                    EV_DEBUG << "checkVmUserFit - The VM: " << i << "has not been handled, not enough space, all the request of the user " << strUserName << "have been deleted" << endl;
//                  }
//                else
//                  {
//                    //Getting VM and scheduling renting timeout
//                    vmRequest.pMsg = scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.nRentTime_t2);
//
//                    //Update value
//                    nAvailableCores = datacenterCollection->getTotalAvailableCores();
//                    EV_DEBUG << "checkVmUserFit - The VM: " << i << " has been handled and stored sucessfully, available cores: "<< nAvailableCores << endl;
//                  }
//              }
//            //Update the data
//            nAvailableCores = datacenterCollection->getTotalAvailableCores();
//            nTotalCores = datacenterCollection->getTotalCores();
//
//            EV_DEBUG << "checkVmUserFit - Updated space#: [" << userVM_Rq->getUserID() << "Requested: "<< nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
//          }
//        else
//          {
//            EV_DEBUG << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserID() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
//            bRet = false;
//          }
//
//        if(bRet)
//            EV_DEBUG << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserID() << endl;
//        else
//            EV_DEBUG << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserID() << endl;
//      }
//    else
//      {
//        EV_ERROR << "checkVmUserFit - WARNING!! nullpointer detected" <<endl;
//        bRet = false;
//      }
//
//    EV_DEBUG << "checkVmUserFit- End" << endl;
//
//    return bRet;
//}

int CloudProviderFirstFit::getPriceByVmType(std::string strPrice)
{
    int nRet;
    VirtualMachine* pVmMachine;

    nRet=0;
    if(strPrice.size()>0)
    {
        for(int i=0;i<vmTypes.size();i++)
        {
            pVmMachine = vmTypes.at(i);
            if(strPrice.compare(pVmMachine->getType()))
            {
                nRet = pVmMachine->getCost();
            }
        }
    }

    return nRet;
}




void  CloudProviderFirstFit::timeoutAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId)
{
    EV_INFO << "Sending timeout to the user:" << userAPP_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " << userAPP_Rq->getLastGateId() << endl;

    SM_UserAPP* userAPP_Res = userAPP_Rq->dup(strVmId);
    userAPP_Res->printUserAPP();

    userAPP_Res->setVmId(strVmId.c_str());

    //Fill the message
    userAPP_Res->setIsResponse(true);
    userAPP_Res->setOperation(SM_APP_Rsp);
    userAPP_Res->setResult(SM_APP_Res_Timeout);

    //Send the values
    sendResponseMessage(userAPP_Res);

}
void  CloudProviderFirstFit::rejectVmRequest(SM_UserVM* userVM_Rq)
{
    //Create a request_rsp message

    EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserID() << endl;

    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Reject);

    //Send the values
    sendResponseMessage(userVM_Rq);
}

void  CloudProviderFirstFit::fillVmFeatures(std::string strVmType, NodeResourceRequest*& pNode)
{
    VirtualMachine* pVmType;

    pVmType = searchVmPerType(strVmType);

    if(pVmType != NULL)
    {
        EV_INFO << "fillVmFeatures - Vm:" << strVmType << " cpus: "<< pVmType->getNumCores() << " mem: " << pVmType->getMemoryGb() <<endl;

        pNode->setTotalCpUs(pVmType->getNumCores());
        pNode->setTotalMemory(pVmType->getMemoryGb());
    }

}
VirtualMachine* CloudProviderFirstFit::searchVmPerType(std::string strVmType)
{
    VirtualMachine* pVM;
    int nIndex;
    bool bFound;

    bFound=false;
    nIndex=0;

    //It is neccesary to search the corresponding VM
    while(nIndex<vmTypes.size()&&!bFound)
    {
        pVM = vmTypes.at(nIndex);
        if(strVmType.compare(pVM->getType()) ==0)
        {
            EV_TRACE << "Vm found: " << pVM->getType() << " vs: "<< strVmType << endl;
            bFound = true;
        }
        nIndex++;
    }
    if(!bFound)
        pVM =NULL;

    return pVM;
}

void CloudProviderFirstFit::handleResponseReject(SIMCAN_Message *sm) {
    sendResponseMessage(sm);
}

void CloudProviderFirstFit::handleResponseAccept(SIMCAN_Message *sm) {
    sendResponseMessage(sm);
}

void CloudProviderFirstFit::handleResponseAppAccept(SIMCAN_Message *sm) {
    updateSubsQueue();
    sendResponseMessage(sm);
}
void CloudProviderFirstFit::handleResponseAppTimeout(SIMCAN_Message *sm) {
    updateSubsQueue();
    sendResponseMessage(sm);
}
void CloudProviderFirstFit::handleResponseNotifySubcription(SIMCAN_Message *sm) {

    SM_UserVM *userVM_Rq;
    SM_UserVM_Finish *pMsg;
    int nIndex;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);

    if(userVM_Rq == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

    nIndex = searchUserInSubQueue(userVM_Rq->getUserID(), userVM_Rq->getStrVmId());
    if(nIndex != -1)
      {
        EV_TRACE << __func__ << " - User found at position:" << nIndex << endl;
        subscribeQueue.erase(subscribeQueue.begin()+nIndex);
      }
    else
      {
        EV_TRACE << __func__ << " - User " << userVM_Rq->getUserID() << " not found in queue list" << endl;
      }


    pMsg = userVM_Rq->getTimeoutSubscribeMsg();
    userVM_Rq->setTimeoutSubscribeMsg(nullptr);
    cancelAndDelete(pMsg);

    sendResponseMessage(sm);
}

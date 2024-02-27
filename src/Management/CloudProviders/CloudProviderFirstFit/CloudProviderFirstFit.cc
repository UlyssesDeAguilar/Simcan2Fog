#include "CloudProviderFirstFit.h"

Define_Module(CloudProviderFirstFit);

CloudProviderFirstFit::~CloudProviderFirstFit()
{
  acceptedUsersRqMap.clear();
}

void CloudProviderFirstFit::initialize()
{
  EV_INFO << "CloudProviderFirstFit::initialize - Init" << '\n';

  // Init super-class
  CloudProviderBase::initialize();

  // Init data-centre structures
  // Fill the meta-structures created to improve the performance of the cloudprovider
  initializeDataCentreCollection();

  // TODO: Meta-data only
  // loadNodes();

  bFinished = false;
  scheduleAt(SimTime(), new cMessage("INITAL_STAGE", INITIAL_STAGE));
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << '\n';
}

void CloudProviderFirstFit::initializeDataCentreCollection()
{
  datacentreCollection = new DataCentreInfoCollection();
}

/**
 * This method initializes the self message handlers
 */
void CloudProviderFirstFit::initializeSelfHandlers()
{
  // ADAA
  selfHandlers[INITIAL_STAGE] = [this](cMessage *msg) -> void
  { return handleInitialStage(msg); };
  // selfHandlers[EXEC_VM_RENT_TIMEOUT] = [this](cMessage *msg) -> void { return handleExecVmRentTimeout(msg); };
  // selfHandlers[EXEC_APP_END] = [this](cMessage *msg) -> void { return handleAppExecEnd(msg); };
  // selfHandlers[EXEC_APP_END_SINGLE] = [this](cMessage *msg) -> void { return handleAppExecEndSingle(msg); };
  selfHandlers[USER_SUBSCRIPTION_TIMEOUT] = [this](cMessage *msg) -> void
  { return handleSubscriptionTimeout(msg); };
  selfHandlers[MANAGE_SUBSCRIBTIONS] = [this](cMessage *msg) -> void
  { return handleManageSubscriptions(msg); };
}

/**
 * This method initializes the request handlers
 */
void CloudProviderFirstFit::initializeRequestHandlers()
{
  requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void
  { return handleVmRequestFits(msg); };
  requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void
  { return handleVmSubscription(msg); };
  requestHandlers[SM_APP_Req] = [this](SIMCAN_Message *msg) -> void
  { return handleUserAppRequest(msg); };
}

void CloudProviderFirstFit::initializeResponseHandlers()
{
  responseHandlers[SM_VM_Res_Accept] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseAccept(msg); };
  responseHandlers[SM_VM_Res_Reject] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseReject(msg); };
  responseHandlers[SM_APP_Res_Accept] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseAppAccept(msg); };
  responseHandlers[SM_APP_Res_Timeout] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseAppTimeout(msg); };
  responseHandlers[SM_APP_Sub_Accept] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseNotifySubcription(msg); };
  responseHandlers[SM_APP_Sub_Reject] = [this](SIMCAN_Message *msg) -> void
  { return handleResponseRejectSubcription(msg); };
  //    responseHandlers[SM_APP_End_Single] = [this](SIMCAN_Message *msg) -> void { return handleResponseAppEndSingle(msg); };
}

void CloudProviderFirstFit::processRequestMessage(SIMCAN_Message *sm)
{
  SM_CloudProvider_Control *userControl;

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << '\n';

  userControl = dynamic_cast<SM_CloudProvider_Control *>(sm);

  if (userControl != nullptr)
  {
    sendRequestMessage(userControl->dup(), toDataCentreGates[0]);
  }
  CloudProviderBase::processRequestMessage(sm);
}

void CloudProviderFirstFit::loadNodes()
{
  DataCentre *pDataCentre;
  Rack *pRack;
  RackInfo *pRackInfo;
  int nComputingRacks, nTotalNodes, nDataCentre, nIndex;

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << '\n';
  // Initialize
  nComputingRacks = nTotalNodes = nDataCentre = nIndex = 0;

  nDataCentre = dataCentresBase.size();

  // Go over the datacentre object: all racks
  if (nDataCentre > 0)
  {
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - handling the DC: " << dataCentresToString() << '\n';
    for (int nIndex = 0; nIndex < nDataCentre; nIndex++)
    {
      EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - loading dataCentre: " << nIndex << '\n';
      pDataCentre = dataCentresBase.at(nIndex);
      if (pDataCentre != nullptr)
      {
        nComputingRacks = pDataCentre->getNumRacks(false);
        EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Number of computing racks: " << nComputingRacks << '\n';
        for (int i = 0; i < nComputingRacks; i++)
        {
          EV_DEBUG << "Getting computing rack: " << i << '\n';

          pRack = pDataCentre->getRack(i, false);

          if (pRack != NULL)
          {
            nTotalNodes = pRack->getNumNodes();

            EV_DEBUG << "Total nodes:" << nTotalNodes << '\n';
            if (nTotalNodes > 0)
            {
              // Insert the Rack
              pRackInfo = pRack->getRackInfo();
              insertRack(nIndex, i, pRackInfo);
            }
            else
            {
              EV_INFO << "WARNING!! The rack is empty :<" << i << '\n';
            }
          }
        }
      }
    }
  }

  // Print the content of the loaded DC
  datacentreCollection->printDCSizes();

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Ende" << '\n';
}
void CloudProviderFirstFit::abortAllApps(SM_UserAPP *userApp, std::string strVmId)
{
  assert((userApp != nullptr));

  userApp->abortAllApps(strVmId);
  // cancelAndDeleteAppFinishMsgs(userApp, strVmId);
}

////Todo: tiene una nota interna. Supongo que se refiere a eliminar el mensaje de otras apps que esten en otro estado.
/*void CloudProviderFirstFit::cancelAndDeleteAppFinishMsgs(SM_UserAPP *userApp, std::string strVmId)
{
  // Terminar este método, para cancelar el resto de eventos si se requiere.
  for (unsigned int nIndex = 0; nIndex < userApp->getAppArraySize(); nIndex++)
  {
    APP_Request &userAppReq = userApp->getApp(nIndex);
    if (userAppReq.vmId.compare(strVmId) == 0 && userAppReq.eState == appFinishedTimeout && userAppReq.pMsgTimeout != nullptr)
    {
      cancelAndDelete(userAppReq.pMsgTimeout);
      userAppReq.pMsgTimeout = nullptr;
    }
  }
}*/

void CloudProviderFirstFit::handleInitialStage(cMessage *msg)
{ // ADAA
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - INITIAL_STAGE" << '\n';
  scheduleAt(simTime() + SimTime(1), new cMessage("MANAGE_SUBSCRIPTIONS", MANAGE_SUBSCRIBTIONS));
}

void CloudProviderFirstFit::handleManageSubscriptions(cMessage *msg)
{
  SM_UserVM *userVmSub;
  std::string strUsername;
  double dWaitingSub, dMaxSubtime;

  if (subscribeQueue.size() < 0)
  {
    EV_TRACE << "The subscription queue is empty" << '\n';
    return;
  }

  EV_TRACE << "The queue of subscription: " << subscribeQueue.size() << '\n';

  // First of all, check the subscriptions timeouts
  for (int i = 0; i < subscribeQueue.size(); i++)
  {
    userVmSub = subscribeQueue.at(i);
    strUsername = userVmSub->getUserId();

    dWaitingSub = (simTime().dbl()) - (userVmSub->getDStartSubscriptionTime());
    dMaxSubtime = userVmSub->getInstanceRequestTimes(0).maxSubTime.dbl();

    if (dWaitingSub > dMaxSubtime)
    {
      EV_INFO << "The subscription of the user:  " << strUsername
              << " has expired, TIMEOUT! SimTime()"
              << simTime().dbl() << " - "
              << userVmSub->getDStartSubscriptionTime()
              << " = | " << dWaitingSub << " vs "
              << dMaxSubtime << '\n';

      // Deallocate VMs
      freeUserVms(strUsername);

      // Remove from queue -> FIXME: This is O(N) ¿Maybe use instead a cQueue?
      subscribeQueue.erase(subscribeQueue.begin() + i);

      // Reset the counter backwards
      i--;

      // Register time and send the Timeout
      userVmSub->setDEndSubscriptionTime(simTime().dbl());
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
               << dWaitingSub - dMaxSubtime << '\n';
    }
  }

  // Check the subscription queue
  updateSubsQueue();
}

void CloudProviderFirstFit::handleSubscriptionTimeout(cMessage *msg)
{
  int nIndex;
  double dWaitingSub, dMaxSubTime;
  SM_UserVM_Finish *pUserSubFinish;
  SM_UserVM *userVmSub;
  std::string strUsername;

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - RECEIVED TIMEOUT " << '\n';
  EV_TRACE << __func__ << " - Init" << '\n';

  nIndex = 0;
  dWaitingSub = dMaxSubTime = 0;
  userVmSub = nullptr;

  if ((pUserSubFinish = dynamic_cast<SM_UserVM_Finish *>(msg)))
  {
    strUsername = pUserSubFinish->getUserID();

    nIndex = searchUserInSubQueue(strUsername, pUserSubFinish->getStrVmId());
    if (nIndex != -1)
    {
      EV_TRACE << __func__ << " - User found at position:" << nIndex << '\n';
      userVmSub = subscribeQueue.at(nIndex);
      userVmSub->setTimeoutSubscribeMsg(nullptr);
      //            if (strcmp(userVmSub->getStrVmId(), "") == 0)
      //                freeUserVms(strUsername);

      dWaitingSub = (simTime().dbl()) - (userVmSub->getDStartSubscriptionTime());
      dMaxSubTime = userVmSub->getInstanceRequestTimes(0).maxSubTime.dbl();

      subscribeQueue.erase(subscribeQueue.begin() + nIndex);

      if (abs((int)dWaitingSub - (int)dMaxSubTime) <= 1)
      {
        EV_INFO << "The subscription of the user:  " << strUsername << " has expired, TIMEOUT! SimTime()" << simTime().dbl() << " - " << userVmSub->getDStartSubscriptionTime() << " = | " << dWaitingSub << " vs " << dMaxSubTime << '\n';

        userVmSub->setDEndSubscriptionTime(simTime().dbl());
        // freeUserVms(strUsername);
        timeoutSubscription(userVmSub);
      }
      else
      {
        EV_TRACE << "The subscription of the user:  " << strUsername << " has time waiting!"
                 << simTime().dbl() << " - " << userVmSub->getDStartSubscriptionTime() << " = | " << dWaitingSub << " vs " << dMaxSubTime << '\n';
      }
      // send the Timeout
    }
    else
    {
      EV_TRACE << __func__ << " - User " << strUsername << " not found in queue list" << '\n';
    }
    // Check the subscription queue
    // updateSubsQueue();
  }
  else
  {
    error("%s - Unable to cast msg to SM_UserVM_Finish*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), msg->getName());
  }

  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - USER_SUBSCRIPTION_TIMEOUT End" << '\n';
}

int CloudProviderFirstFit::searchUserInSubQueue(std::string strUsername, std::string strVmId)
{
  EV_TRACE << __func__ << " - Searching for user: " << strUsername << '\n';

  // The condition for finding the user
  auto selector = [strUsername, strVmId](SM_UserVM *vm) -> bool
  {
    return strUsername.compare(vm->getUserId()) == 0 &&
           (strcmp(vm->getVmId(), "") == 0 || strVmId.compare(vm->getVmId()) == 0);
  };

  // Attempt the search
  auto position = std::find_if(subscribeQueue.begin(), subscribeQueue.end(), selector);

  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << '\n';

  // If found calculate the index (this only works for a vector)
  if (position != subscribeQueue.end())
    return position - subscribeQueue.begin();

  // If not found
  return -1;
}
void CloudProviderFirstFit::updateSubsQueue()
{
  SM_UserVM *userVmSub;

  // Finally, notify if there is enough space to handle a new notification
  for (int i = 0; i < subscribeQueue.size(); i++)
  {
    userVmSub = subscribeQueue.at(i);

    userVmSub->setOperation(SM_VM_Sub);
    // userVmSub->setResult(0);

    // If the message is not in the datacentre yet
    if (userVmSub->getOwner() == this && userVmSub->getResult() != SM_APP_Sub_Accept)
    {
      checkVmUserFit(userVmSub);
      EV_INFO << "Notifying subscription of user: " << userVmSub->getUserId() << '\n';
    }
  }
}

void CloudProviderFirstFit::freeUserVms(std::string strUsername)
{

  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << '\n';
  SM_UserVM *pVmRequest = getOrNull(acceptedUsersRqMap, strUsername);

  // FIXME: Maybe warn the user ?
  if (pVmRequest == nullptr)
    return;

  // Mark the user VMs as free
  for (int i = 0; i < pVmRequest->getVmArraySize(); i++)
  {
    auto vmRequest = pVmRequest->getVm(i);
    std::string strVmId = vmRequest.vmId;
    freeVm(strVmId);
  }

  acceptedUsersRqMap.erase(strUsername);

  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << '\n';
}

void CloudProviderFirstFit::freeVm(std::string strVmId)
{
  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << '\n';
  if (!strVmId.empty())
  {
    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Releasing the Vm:  " << strVmId << '\n';

    if (datacentreCollection->freeVmRequest(strVmId))
      EV_DEBUG << "the Vm has been released sucessfully: " << strVmId << '\n';
    else // Error freeing the VM
      EV_INFO << "Error releasing the VM: " << strVmId << '\n';
  }

  EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << '\n';
}

void CloudProviderFirstFit::insertRack(int nIndex, int nRack, RackInfo *pRackInfo)
{
  NodeInfo *pNodeInfo;
  NodeResourceInfo *pNodeResInfo;
  int nTotalCpus, nTotalMemory, cpuSpeed, nTotalNodes;
  double totalDiskGB, totalMemoryGB;
  bool storage;

  storage = false;
  nTotalCpus = nTotalMemory = cpuSpeed = nTotalNodes = 0;

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init " << '\n';

  if (pRackInfo != NULL)
  {
    pNodeInfo = pRackInfo->getNodeInfo();
    if (pNodeInfo != NULL)
    {
      // Retrieve all the values
      nTotalCpus = pNodeInfo->getNumCpUs();
      nTotalMemory = pNodeInfo->getTotalMemoryGb();
      cpuSpeed = pNodeInfo->getCpuSpeed();
      totalDiskGB = pNodeInfo->getTotalDiskGb();
      totalMemoryGB = pNodeInfo->getTotalMemoryGb();
      storage = pNodeInfo->isStorage();
      nTotalNodes = pRackInfo->getNumBoards() * pRackInfo->getNodesPerBoard();

      for (int j = 0; j < nTotalNodes; j++)
      {
        EV_TRACE << "Adding computing node : " << j << " | rack: " << nRack << '\n';

        pNodeResInfo = new NodeResourceInfo();
        pNodeResInfo->setNumCpUs(nTotalCpus);
        pNodeResInfo->setAvailableCpUs(nTotalCpus);
        pNodeResInfo->setTotalMemoryGb(nTotalMemory);
        pNodeResInfo->setAvailableMemory(nTotalMemory);
        pNodeResInfo->setCpuSpeed(cpuSpeed);
        pNodeResInfo->setTotalDiskGb(totalDiskGB);
        pNodeResInfo->setAvailableDiskGb(totalDiskGB);

        pNodeResInfo->setIp("dc:" + std::to_string(nIndex) + "_rack:" + std::to_string(nRack) + "_node:" + std::to_string(j));
        pNodeResInfo->setDataCentre(nIndex);

        EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Inserting node " << j << " at datacentre: " << nIndex << '\n';
        // Insert the node info into the corresponding DC
        datacentreCollection->insertNode(nIndex, pNodeResInfo);
      }
    }
  }

  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - End " << '\n';
}

void CloudProviderFirstFit::handleVmRequestFits(SIMCAN_Message *sm)
{
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << '\n';

  auto userVM_Rq = dynamic_cast<SM_UserVM *>(sm);
  assert_msg(userVM_Rq != nullptr, "Wrong userVM_Rq. Null pointer or bad operation code!");

  // Log user request
  EV_INFO << userVM_Rq << '\n';

  // Check if is a VmRequest or a subscribe
  if (subscribeQueue.size() > 0)
    rejectVmRequest(userVM_Rq);
  else if (checkVmUserFit(userVM_Rq))
  {
    // acceptVmRequest(userVM_Rq);
    EV_FATAL << "OK" << '\n';
  }
  else
  {
    EV_FATAL << "Fail" << '\n';
    rejectVmRequest(userVM_Rq);
  }
}

void CloudProviderFirstFit::handleVmSubscription(SIMCAN_Message *sm)
{
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Subscribe operation" << '\n';

  auto userVM_Rq = dynamic_cast<SM_UserVM *>(sm);
  assert_msg(userVM_Rq != nullptr, "Wrong userVM_Rq. Null pointer or bad operation code!");

  storeVmSubscribe(userVM_Rq);
  updateSubsQueue();
}

void CloudProviderFirstFit::handleUserAppRequest(SIMCAN_Message *sm)
{
  // Get the user name, and recover the info about the VmRequests;
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle AppRequest" << '\n';

  auto userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);
  assert_msg(userAPP_Rq != nullptr, "Wrong userAPP_Rq. Nullpointer!!");

  // TODO: Select datacentre
  sendRequestMessage(userAPP_Rq, toDataCentreGates[0]);
}

void CloudProviderFirstFit::handleResponseRejectSubcription(SIMCAN_Message *sm)
{
  SM_UserVM *userVM_Rq = dynamic_cast<SM_UserVM *>(sm);
  EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << '\n';

  if (userVM_Rq == nullptr)
    throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

  // storeVmSubscribe(userVM_Rq);
}

void CloudProviderFirstFit::notifySubscription(SM_UserVM *userVM_Rq)
{
  SM_UserVM_Finish *pMsgTimeout;

  EV_INFO << "Notifying request from user: " << userVM_Rq->getUserId() << '\n';
  EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';

  // Fill the message
  userVM_Rq->setIsResponse(true);
  userVM_Rq->setOperation(SM_VM_Notify);
  userVM_Rq->setResult(SM_APP_Sub_Accept);

  // Cancel the timeout event
  pMsgTimeout = userVM_Rq->getTimeoutSubscribeMsg();
  userVM_Rq->setTimeoutSubscribeMsg(nullptr);
  cancelAndDelete(pMsgTimeout);

  // Send the values
  sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFit::timeoutSubscription(SM_UserVM *userVM_Rq)
{
  EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserId() << '\n';
  EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';

  userVM_Rq->setTimeoutSubscribeMsg(nullptr);

  // Fill the message
  userVM_Rq->setIsResponse(true);
  userVM_Rq->setOperation(SM_VM_Notify);
  userVM_Rq->setResult(SM_APP_Sub_Timeout);

  // Send the values
  sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFit::storeVmSubscribe(SM_UserVM *userVM_Rq)
{
  double dMaxSubscribeTime;
  SM_UserVM_Finish *pMsg;
  std::string strUserName;

  if (userVM_Rq != nullptr)
  {
    strUserName = userVM_Rq->getUserId();
    dMaxSubscribeTime = userVM_Rq->getInstanceRequestTimes(0).maxSubTime.dbl();
    EV_INFO << "Subscribing the VM request from user:" << strUserName << " | max sub time: " << dMaxSubscribeTime << '\n';

    pMsg = userVM_Rq->getTimeoutSubscribeMsg();
    if (pMsg == nullptr)
    {
      // FIXME: In CloudManager base maybe make this method more specific
      pMsg = scheduleVmMsgTimeout(USER_SUBSCRIPTION_TIMEOUT, userVM_Rq, dMaxSubscribeTime);

      // Store the VM subscription until there exist the Vms necessaries
      userVM_Rq->setDStartSubscriptionTime(simTime().dbl());
      userVM_Rq->setTimeoutSubscribeMsg(pMsg);
    }

    subscribeQueue.push_back(userVM_Rq);

    // scheduleAt(simTime() + SimTime(dMaxSubscribeTime), pMsg);
  }
}

void CloudProviderFirstFit::clearVMReq(SM_UserVM *&userVM_Rq, int lastId)
{
  for (int i = 0; i < lastId; i++)
  {
    auto vmRequest = userVM_Rq->getVm(i);
    cancelAndDelete(vmRequest.pMsg);
    vmRequest.pMsg = nullptr;
    datacentreCollection->freeVmRequest(vmRequest.vmId);
  }
}

bool CloudProviderFirstFit::checkVmUserFit(SM_UserVM *&userVM_Rq)
{
  int nTotalAvailableCores = dataCentreManagers[0]->getNTotalAvailableCores();
  int nTotalCoresRequested = calculateTotalCoresRequested(userVM_Rq);

  // If there aren't enough resources
  if (nTotalAvailableCores < nTotalCoresRequested)
    return false;

  // TODO: Select datacentre
  // Relay the request to the corresonding DC
  userVM_Rq->setIsResponse(false);
  sendRequestMessage(userVM_Rq, toDataCentreGates[0]);
  return true;
}

int CloudProviderFirstFit::getPriceByVmType(const std::string &vmType)
{
  auto vm = dataManager->searchVirtualMachine(vmType);
  return vm == nullptr ? vm->getCost() : 0;
}

void CloudProviderFirstFit::timeoutAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId)
{
  EV_INFO << "Sending timeout to the user:" << userAPP_Rq->getUserID() << '\n';
  EV_INFO << "Last id gate: " << userAPP_Rq->getLastGateId() << '\n';

  SM_UserAPP *userAPP_Res = userAPP_Rq->dup(strVmId);
  EV_INFO << *userAPP_Res << "\n";

  userAPP_Res->setVmId(strVmId.c_str());

  // Fill the message
  userAPP_Res->setIsResponse(true);
  userAPP_Res->setOperation(SM_APP_Rsp);
  userAPP_Res->setResult(SM_APP_Res_Timeout);

  // Send the values
  sendResponseMessage(userAPP_Res);
}
void CloudProviderFirstFit::rejectVmRequest(SM_UserVM *userVM_Rq)
{
  // Create a request_rsp message

  EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserId() << '\n';

  userVM_Rq->setIsResponse(true);
  userVM_Rq->setOperation(SM_VM_Req_Rsp);
  userVM_Rq->setResult(SM_VM_Res_Reject);

  // Send the values
  sendResponseMessage(userVM_Rq);
}

void CloudProviderFirstFit::fillVmFeatures(std::string strVmType, NodeResourceRequest *&pNode)
{
  auto pVmType = dataManager->searchVirtualMachine(strVmType);

  if (pVmType != nullptr)
  {
    EV_INFO << "fillVmFeatures - Vm:" << strVmType << " cpus: " << pVmType->getNumCores() << " mem: " << pVmType->getMemoryGb() << '\n';

    pNode->setTotalCpUs(pVmType->getNumCores());
    pNode->setTotalMemory(pVmType->getMemoryGb());
  }
}

void CloudProviderFirstFit::handleResponseReject(SIMCAN_Message *sm)
{
  sendResponseMessage(sm);
}

void CloudProviderFirstFit::handleResponseAccept(SIMCAN_Message *sm)
{
  sendResponseMessage(sm);
}

void CloudProviderFirstFit::handleResponseAppAccept(SIMCAN_Message *sm)
{
  updateSubsQueue();
  sendResponseMessage(sm);
}
void CloudProviderFirstFit::handleResponseAppTimeout(SIMCAN_Message *sm)
{
  updateSubsQueue();
  sendResponseMessage(sm);
}
void CloudProviderFirstFit::handleResponseNotifySubcription(SIMCAN_Message *sm)
{

  SM_UserVM *userVM_Rq;
  SM_UserVM_Finish *pMsg;
  int nIndex;

  userVM_Rq = dynamic_cast<SM_UserVM *>(sm);

  if (userVM_Rq == nullptr)
    throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

  nIndex = searchUserInSubQueue(userVM_Rq->getUserId(), userVM_Rq->getVmId());
  if (nIndex != -1)
  {
    EV_TRACE << __func__ << " - User found at position:" << nIndex << '\n';
    subscribeQueue.erase(subscribeQueue.begin() + nIndex);
  }
  else
  {
    EV_TRACE << __func__ << " - User " << userVM_Rq->getUserId() << " not found in queue list" << '\n';
  }

  pMsg = userVM_Rq->getTimeoutSubscribeMsg();
  userVM_Rq->setTimeoutSubscribeMsg(nullptr);
  cancelAndDelete(pMsg);

  sendResponseMessage(sm);
}

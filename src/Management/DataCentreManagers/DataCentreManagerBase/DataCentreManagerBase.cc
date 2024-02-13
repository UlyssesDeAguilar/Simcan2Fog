#include "DataCentreManagerBase.h"

#include "Management/utils/LogUtils.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"

using namespace hypervisor;

// Define_Module(DataCentreManagerBase);

DataCentreManagerBase::~DataCentreManagerBase()
{
    acceptedVMsMap.clear();
    acceptedUsersRqMap.clear();
    handlingAppsRqMap.clear();
    mapAppsVectorModulePerVm.clear();
    mapAppsModulePerId.clear();
    mapAppsRunningInVectorModulePerVm.clear();
}

void DataCentreManagerBase::initialize()
{
    // Init super-class
    CloudManagerBase::initialize();

    // Turn off accepting responses
    acceptResponses = false;

    // Link app builder
    appBuilder = new DataCentreApplicationBuilder();
    appBuilder->setManager(this);

    // FIXME: The method parseConfig() no longer exists -- see implications
    if (parseDataCentreConfig() == SC_ERROR)
        error("Error while parsing data-centres list");

    // Get parameters from module
    showDataCentreConfig = par("showDataCentreConfig");
    nCpuStatusInterval = par("cpuStatusInterval");
    nActiveMachinesThreshold = par("activeMachinesThreshold");
    nMinActiveMachines = par("minActiveMachines");
    forecastActiveMachines = par("forecastActiveMachines");

    // Get gates
    networkInGateId = gate("networkIn")->getId();
    localInGateId = gateHalf("localNetwork", cGate::INPUT)->getId();

    // Create and schedule auto events
    cpuManageMachinesMessage = new cMessage("MANAGE_MACHINES", MANAGE_MACHINES);
    cpuStatusMessage = new cMessage("CPU_STATUS", CPU_STATUS);
    scheduleAt(SimTime(), cpuManageMachinesMessage);
    scheduleAt(SimTime(), cpuStatusMessage);
}

void DataCentreManagerBase::finish()
{
    // Delete the events
    cancelAndDelete(cpuManageMachinesMessage);
    cancelAndDelete(cpuStatusMessage);
    delete appBuilder;

    // Print statistics
    printFinal();
}

int DataCentreManagerBase::parseDataCentreConfig()
{
    int result;
    const char *dataCentresConfigChr;

    dataCentresConfigChr = par("dataCentreConfig");
    DataCentreConfigParser dataCentreParser(dataCentresConfigChr);
    result = dataCentreParser.parse();
    if (result == SC_OK)
    {
        dataCentresBase = dataCentreParser.getResult();
    }
    return result;
}

cGate *DataCentreManagerBase::getOutGate(cMessage *msg)
{
    int gateId = msg->getArrivalGate()->getId();

    // Check from which network the request came from
    if (gateId == networkInGateId)
        return gate("networkOut");
    else if (gateId == localInGateId)
        return gateHalf("localNetwork", cGate::OUTPUT);
    else
        error("Message received from an unknown gate [%s]", msg->getName());

    return nullptr;
}

/**
 * This method initializes the self message handlers
 */
void DataCentreManagerBase::initializeSelfHandlers()
{
    // ADAA
    //    selfHandlers[INITIAL_STAGE] = [this](cMessage *msg) -> void { return handleInitialStage(msg); };
    selfHandlers[EXEC_VM_RENT_TIMEOUT] = [this](cMessage *msg) -> void
    { return handleExecVmRentTimeout(msg); };
    // selfHandlers[EXEC_APP_END] = [this](cMessage *msg) -> void { return handleAppExecEnd(msg); };
    //    selfHandlers[EXEC_APP_END_SINGLE] = [this](cMessage *msg) -> void { return handleAppExecEndSingle(msg); };
    //    selfHandlers[USER_SUBSCRIPTION_TIMEOUT] = [this](cMessage *msg) -> void { return handleSubscriptionTimeout(msg); };
    //    selfHandlers[MANAGE_SUBSCRIBTIONS] = [this](cMessage *msg) -> void { return handleManageSubscriptions(msg); };
    selfHandlers[CPU_STATUS] = [this](cMessage *msg) -> void
    { return handleCpuStatus(msg); };
    selfHandlers[MANAGE_MACHINES] = [this](cMessage *msg) -> void
    { return handleManageMachines(msg); };
}

/**
 * This method initializes the request handlers
 */
void DataCentreManagerBase::initializeRequestHandlers()
{
    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRequestFits(msg); };
    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void
    { return handleVmSubscription(msg); };
    requestHandlers[SM_APP_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleUserAppRequest(msg); };
}

void DataCentreManagerBase::handleCpuStatus(cMessage *msg)
{
    const char *strName = getParentModule()->getName();
    EV_FATAL << "#___cpuUsage#" << strName << " " << simTime().dbl() << " " << resourceManager->getAggregatedCpuUsage() << "\n";

    // If not finished, reuse the same message CPU_STATUS
    if (!bFinished)
    {
        scheduleAt(simTime() + SimTime(nCpuStatusInterval, SIMTIME_S), msg);
    }
}

void DataCentreManagerBase::handleManageMachines(cMessage *msg)
{
    const char *strName = getParentModule()->getName();
    int machinesInUseForecasting;
    int machinesInUse = resourceManager->getMachinesInUse();
    int activeMachines = resourceManager->getActiveMachines();
    int activeOrInUseMachines = resourceManager->getActiveOrInUseMachines();
    
    EV_FATAL << "#___machinesInUse#" << strName << " " << simTime().dbl() << " " << machinesInUse << '\n';
    EV_FATAL << "#___activeMachines#" << strName << " " << simTime().dbl() << " " << activeMachines << '\n';
    EV_FATAL << "#___activeOrInUseMachines#" << strName << " " << simTime().dbl() << " " << activeOrInUseMachines << '\n';

    // If not finished, reuse the same message MANAGE_MACHINES
    if (!bFinished)
        scheduleAt(simTime() + SimTime(nCpuStatusInterval, SIMTIME_S), msg);

    if (forecastActiveMachines)
    {
        currForecastingQuery[0] = machinesInUse;
        smthForecastingResult = timeSeriesForecasting.push_to_pop(currForecastingQuery);

        machinesInUseForecasting = smthForecastingResult[0];
        EV_FATAL << "#___machinesInUseForecast#" << strName << " " << (simTime() + SimTime(nCpuStatusInterval, SIMTIME_S)).dbl()
                 << " " << machinesInUseForecasting << '\n';

        nLastMachinesInUseForecasting = machinesInUseForecasting;
        if (machinesInUseForecasting + nActiveMachinesThreshold > machinesInUse)
            resourceManager->setActiveMachines(machinesInUseForecasting + nActiveMachinesThreshold);
    }
}

void DataCentreManagerBase::manageActiveMachines()
{
    int machinesInUse = resourceManager->getMachinesInUse();

    if (forecastActiveMachines)
    {
        int activeMachines = resourceManager->getActiveMachines();
        if (machinesInUse + nActiveMachinesThreshold > activeMachines)
        {
            resourceManager->setActiveMachines(machinesInUse + nActiveMachinesThreshold);
        }
    }
    else if (!forecastActiveMachines && nActiveMachinesThreshold > 0)
    {
        resourceManager->setActiveMachines(machinesInUse + nActiveMachinesThreshold);
    }
}

void DataCentreManagerBase::handleUserAppRequest(SIMCAN_Message *sm)
{
    // TODO -- forward to the hypervisor to do the deployment
}

void DataCentreManagerBase::handleVmRequestFits(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << '\n';

    auto userVM_Rq = check_and_cast<SM_UserVM *>(sm);
    EV_INFO << userVM_Rq << '\n';

    // Check if is a VmRequest or a subscribe
    if (checkVmUserFit(userVM_Rq))
        acceptVmRequest(userVM_Rq);
    else
        rejectVmRequest(userVM_Rq);
}

void DataCentreManagerBase::handleVmSubscription(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Subscribe operation" << '\n';

    auto userVM_Rq = check_and_cast<SM_UserVM *>(sm);

    if (checkVmUserFit(userVM_Rq))
        notifySubscription(userVM_Rq);
    else
        rejectVmSubscribe(userVM_Rq); // Store the vmRequest
}

void DataCentreManagerBase::rejectVmSubscribe(SM_UserVM *userVM_Rq)
{
    EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserId() << '\n';
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';

    // Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Reject);

    // Send the values
    sendResponseMessage(userVM_Rq);
}

void DataCentreManagerBase::notifySubscription(SM_UserVM *userVM_Rq)
{
    SM_UserVM_Finish *pMsgTimeout;
    EV_INFO << "Notifying request from user: " << userVM_Rq->getUserId() << '\n';
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';

    // Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Accept);

    // Cancel the timeout event
    //    pMsgTimeout = userVM_Rq->getTimeoutSubscribeMsg();
    //    if(pMsgTimeout != nullptr)
    //    {
    //        cancelAndDelete(pMsgTimeout);
    //        userVM_Rq->setTimeoutSubscribeMsg(nullptr);
    //    }

    // Send the values
    sendResponseMessage(userVM_Rq);
}

void DataCentreManagerBase::acceptVmRequest(SM_UserVM *userVM_Rq)
{
    EV_INFO << "Accepting request from user:" << userVM_Rq->getUserId() << '\n';

    //    if(acceptedUsersRqMap.find(userVM_Rq->getUserId()) == acceptedUsersRqMap.end())
    //    {
    //        //Accepting new user
    //        EV_INFO << "Registering new user in the system:" << userVM_Rq->getUserId() << '\n';
    //        acceptedUsersRqMap[userVM_Rq->getUserId()] = userVM_Rq;
    //    }

    // Fill the message
    // userVM_Rq->setIsAccept(true);
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Accept);

    // Send the values
    sendResponseMessage(userVM_Rq);
}

void DataCentreManagerBase::rejectVmRequest(SM_UserVM *userVM_Rq)
{
    // Create a request_rsp message

    EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserId() << '\n';

    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Reject);

    // Send the values
    sendResponseMessage(userVM_Rq);
}

bool DataCentreManagerBase::checkVmUserFit(SM_UserVM *&userVM_Rq)
{
    assert_msg(userVM_Rq != nullptr, "checkVmUserFit - WARNING!! nullpointer detected\n");

    std::string nodeIp, strVmId;
    int nRequestedVms = userVM_Rq->getVmArraySize();
    std::string userId = userVM_Rq->getUserId();

    EV_DEBUG << "checkVmUserFit- Init" << '\n';
    EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userId << '\n';

    // Before starting the process, it is neccesary to check if the
    int nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
    int nAvailableCores = getNTotalAvailableCores();

    if (nTotalRequestedCores > nAvailableCores)
    {
        EV_DEBUG << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserId() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << getNTotalCores() << "]" << '\n';
        return false;
    }

    int nTotalCores = getNTotalCores();
    EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserId() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';

    // Process all the VMs
    bool bRet, bAccepted;
    bAccepted = bRet = true;

    for (int i = 0; i < nRequestedVms && bRet; i++)
    {
        EV_DEBUG << "\ncheckVmUserFit - Trying to handle the VM: " << i << '\n';

        // Get the VM request
        auto vmRequest = userVM_Rq->getVm(i);

        DcHypervisor *hypervisor = selectNode(userVM_Rq, vmRequest);

        if (hypervisor != nullptr)
        {
            acceptedVMsMap[vmRequest.vmId] = hypervisor;
            nodeIp = hypervisor->getFullPath();
            bAccepted = true;
        }
        else
        {
            bAccepted = false;
        }

        bRet &= bAccepted;

        userVM_Rq->createResponse(i, bRet, simTime().dbl(), nodeIp, 0);

        if (!bRet)
        {
            clearVMReq(userVM_Rq, i);
            // EV_DEBUG << "checkVmUserFit - The VM: " << i << "has not been handled, not enough space, all the request of the user " << strUserName << "have been deleted" << '\n';
        }
        else
        {
            double rentTime;
            // Getting VM and scheduling renting timeout
            strVmId = userVM_Rq->getVmId();
            if (!strVmId.empty() && userVM_Rq->getOperation() == SM_VM_Sub)
                rentTime = 3600;
            else
                rentTime = vmRequest.times.rentTime.dbl();

            // vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.strVmType, nRentTime);
            vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, userId, vmRequest, rentTime);
        }

        EV_DEBUG << "checkVmUserFit - Updated space#: [" << userVM_Rq->getUserId() << "Requested: " << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';
    }

    if (bRet)
    {
        acceptedUsersRqMap[userId] = userVM_Rq;
        // nTotalAvailableCores -= nTotalRequestedCores; TODO FIX
        EV_DEBUG << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserId() << '\n';
    }
    else
    {
        EV_DEBUG << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserId() << '\n';
    }

    EV_DEBUG << "checkVmUserFit- End" << '\n';

    return bRet;
}

void DataCentreManagerBase::deallocateVmResources(std::string strVmId)
{
    DcHypervisor *pHypervisor = getNodeHypervisorByVm(strVmId);

    if (pHypervisor == nullptr)
        error("%s - Unable to deallocate VM. Wrong VM name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strVmId.c_str());

    pHypervisor->deallocateVmResources(strVmId);
    manageActiveMachines();
}

void DataCentreManagerBase::clearVMReq(SM_UserVM *&userVM_Rq, int lastId)
{
    for (int i = 0; i < lastId; i++)
    {
        auto vmRequest = userVM_Rq->getVm(i);
        cancelAndDelete(vmRequest.pMsg);
        vmRequest.pMsg = nullptr;

        deallocateVmResources(vmRequest.vmId);

        // datacentreCollection->freeVmRequest(vmRequest.strVmId);
    }
}

bool DataCentreManagerBase::allocateVM(const VM_Request &vm, hypervisor::DcHypervisor *pHypervisor)
{
    // TODO: Probablemente sea mejor mover esto al hypervisor. La asignaci�n al map y que sea el hypervisor el que controle a que VM va.
    // TODO: Finalmente deber�a devolver la IP del nodo y que el mensaje de la App llegue al nodo.
    cModule *pVmAppVectorModule = pHypervisor->handleVmRequest(vm);
    if (pVmAppVectorModule != nullptr)
    {
        // updateCpuUtilizationTimeForHypervisor(pHypervisor);
        mapAppsVectorModulePerVm[vm.vmId] = pVmAppVectorModule;
        int numMaxApps = pVmAppVectorModule->par("numApps");
        bool *runningAppsArr = new bool[numMaxApps]{false};
        mapAppsRunningInVectorModulePerVm[vm.vmId] = runningAppsArr;
        return true;
    }
    return false;
}

void DataCentreManagerBase::printFinal()
{
    simtime_t finalSimulationTime = simTime();
    const char *strName = getParentModule()->getName();
    int nTotalIndex = 0;

    /* Printing statistics --> Should move this probably to Resource Manager!

    for (const auto &entry : mapCpuUtilizationTimePerHypervisor)
    {
        auto timersTuple = entry.second;
        uint32_t numCores = std::get<0>(timersTuple);
        simtime_t *timerArray = std::get<2>(timersTuple);

        for (int i = 0; i < numCores; i++)
        {
            EV_FATAL << "#___cpuTime#" << strName << " " << nTotalIndex << " " << timerArray[i].dbl()
                     << " " << timerArray[i].dbl() / finalSimulationTime.dbl() << '\n';
            nTotalIndex++;
        }
    }
    */
}

#include "DataCentreManagerBase.h"

#include "Management/utils/LogUtils.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"

// Define_Module(DataCentreManagerBase);

DataCentreManagerBase::~DataCentreManagerBase()
{
    acceptedVMsMap.clear();
    acceptedUsersRqMap.clear();
    handlingAppsRqMap.clear();
    mapHypervisorPerNodes.clear();
    mapAppsVectorModulePerVm.clear();
    mapAppsModulePerId.clear();
    mapAppsRunningInVectorModulePerVm.clear();
    mapCpuUtilizationTimePerHypervisor.clear();
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

    // Init variables
    nTotalCores = nTotalAvailableCores = nTotalMachines = nTotalActiveMachines = 0;

    // Get parameters from module
    showDataCentreConfig = par("showDataCentreConfig");
    nCpuStatusInterval = par("cpuStatusInterval");
    nActiveMachinesThreshold = par("activeMachinesThreshold");
    nMinActiveMachines = par("minActiveMachines");
    forecastActiveMachines = par("forecastActiveMachines");

    // Get gates
    inGate = gate("in");
    outGate = gate("out");

    // Parse data-centre list
    // TODO: Meta-data only

    int result = initDataCentreMetadata();

    // Something goes wrong...
    if (result == SC_ERROR)
    {
        error("Error while parsing data-centres (initialize) list");
    }
    else if (showDataCentreConfig)
    {
        EV_DEBUG << dataCentreToString();
    }

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

int DataCentreManagerBase::initDataCentreMetadata()
{
    int result = SC_OK;
    // DataCentre *pDataCentreBase = dataCentresBase[0];
    cModule *pRackModule;
    int numTotalCores = 0;

    cModule *submodule;
    std::string subModuleName;
    std::size_t found;
    int vectorSize;

    for (cModule::SubmoduleIterator it(getParentModule()); !it.end(); ++it)
    {
        submodule = *it;
        subModuleName = std::string(submodule->getName());
        const char *nameP = subModuleName.c_str();
        found = subModuleName.find("rackCmp_");
        if (found != std::string::npos)
        {
            numTotalCores += storeRackMetadata(submodule);
        }
    }

    //    for (int nRackIndex=0; nRackIndex < pDataCentreBase->getNumRacks(false); nRackIndex++ ) {
    //        Rack* pRackBase = pDataCentreBase->getRack(nRackIndex,false);
    //        //Generate rack name in the data centre
    //        string strRackName = "rackCmp_" + pRackBase->getRackInfo()->getName();
    //        pRackModule = getParentModule()->getSubmodule(strRackName.c_str(), nRackIndex);
    //
    //        numBoards =  pRackModule->par("numBoards");
    //
    //        for (int nBoardIndex=0; nBoardIndex < numBoards; nBoardIndex++) {
    //            pBoardModule = pRackModule->getSubmodule("board", nBoardIndex);
    //            numNodes = pBoardModule->par("numBlades");
    //            numTotalCores+=numNodes;
    //
    //            for (int nNodeIndex=0; nNodeIndex < numNodes; nNodeIndex++) {
    //                pNodeModule = pBoardModule->getSubmodule("blade", nNodeIndex);
    //
    //                storeNodeMetadata(pNodeModule);
    //
    //            }
    //        }
    //
    //
    //    }

    nTotalCores = nTotalAvailableCores = numTotalCores;

    return result;
}

int DataCentreManagerBase::storeRackMetadata(cModule *pRackModule)
{
    int numBoards, numTotalCores, numNodes;
    cModule *pBoardModule, *pNodeModule;

    numTotalCores = 0;
    numBoards = pRackModule->par("numBoards");

    for (int nBoardIndex = 0; nBoardIndex < numBoards; nBoardIndex++)
    {
        pBoardModule = pRackModule->getSubmodule("board", nBoardIndex);
        numNodes = pBoardModule->par("numBlades");

        for (int nNodeIndex = 0; nNodeIndex < numNodes; nNodeIndex++)
        {
            pNodeModule = pBoardModule->getSubmodule("blade", nNodeIndex);
            numTotalCores += storeNodeMetadata(pNodeModule);
        }
    }

    return numTotalCores;
}

int DataCentreManagerBase::storeNodeMetadata(cModule *pNodeModule)
{
    cModule *pHypervisorModule;
    Hypervisor *pHypervisor;
    int numCores;

    pHypervisorModule = pNodeModule->getSubmodule("osModule")->getSubmodule("hypervisor");

    numCores = pNodeModule->par("numCpuCores");

    pHypervisor = check_and_cast<Hypervisor *>(pHypervisorModule);

    simtime_t **startTimeArray = new simtime_t *[numCores];
    simtime_t *timerArray = new simtime_t[numCores];
    for (int i = 0; i < numCores; i++)
    {
        startTimeArray[i] = nullptr;
        timerArray[i] = SimTime();
    }

    nTotalMachines++;
    if (pHypervisor->isActive())
        nTotalActiveMachines++;

    // Store hypervisor pointers by number of cores
    mapHypervisorPerNodes[numCores].push_back(pHypervisor);
    // Initialize cpu utilization timers
    mapCpuUtilizationTimePerHypervisor[pHypervisorModule->getFullPath()] = std::make_tuple(numCores, startTimeArray, timerArray);

    return numCores;
}

int DataCentreManagerBase::getMachinesInUse()
{
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::vector<Hypervisor *>::iterator itVector;
    int nMachinesInUse;

    nMachinesInUse = 0;

    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end(); itMap++)
    {
        for (itVector = itMap->second.begin(); itVector != itMap->second.end(); itVector++)
        {
            if ((*itVector)->isInUse())
            {
                nMachinesInUse++;
            }
        }
    }

    return nMachinesInUse;
}

int DataCentreManagerBase::getActiveMachines()
{
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::vector<Hypervisor *>::iterator itVector;
    int nActiveMachines;

    nActiveMachines = 0;

    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end(); itMap++)
    {
        for (itVector = itMap->second.begin(); itVector != itMap->second.end(); itVector++)
        {
            if ((*itVector)->isActive())
            {
                nActiveMachines++;
            }
        }
    }

    return nActiveMachines;
}

int DataCentreManagerBase::getActiveOrInUseMachines()
{
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::vector<Hypervisor *>::iterator itVector;
    int nActiveOrInUseMachines;

    nActiveOrInUseMachines = 0;

    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end(); itMap++)
    {
        for (itVector = itMap->second.begin(); itVector != itMap->second.end(); itVector++)
        {
            if ((*itVector)->isInUse() || (*itVector)->isActive())
            {
                nActiveOrInUseMachines++;
            }
        }
    }

    return nActiveOrInUseMachines;
}

void DataCentreManagerBase::setActiveMachines(int nNewActiveMachines)
{
    Hypervisor *pHypervisor = nullptr;
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::map<int, std::vector<Hypervisor *>>::reverse_iterator ritMap;
    std::vector<Hypervisor *>::iterator itVector;
    int nActiveMachines = getActiveMachines();

    if (nNewActiveMachines < nMinActiveMachines)
        nNewActiveMachines = nMinActiveMachines;

    if (nActiveMachines < nNewActiveMachines)
    {
        for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end() && nActiveMachines < nNewActiveMachines; ++itMap)
        {
            std::vector<Hypervisor *> &vectorHypervisor = itMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && nActiveMachines < nNewActiveMachines; ++itVector)
            {
                pHypervisor = *itVector;
                if (!pHypervisor->isActive())
                {
                    pHypervisor->powerOn(true);
                    nActiveMachines++;
                }
            }
        }
    }
    else if (nActiveMachines > nNewActiveMachines)
    {
        for (ritMap = mapHypervisorPerNodes.rbegin(); ritMap != mapHypervisorPerNodes.rend() && nActiveMachines > nNewActiveMachines; ++ritMap)
        {
            std::vector<Hypervisor *> &vectorHypervisor = ritMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && nActiveMachines > nNewActiveMachines; ++itVector)
            {
                pHypervisor = *itVector;
                if (pHypervisor->isActive())
                {
                    pHypervisor->powerOn(false);
                    nActiveMachines--;
                }
            }
        }
    }
}

double DataCentreManagerBase::getCurrentCpuPercentageUsage()
{
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::vector<Hypervisor *>::iterator itVector;
    int nTotalCores, nUsedCores, nNodeCores;
    double dPercentage = 0.0;

    nTotalCores = nUsedCores = 0;

    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end(); itMap++)
    {
        for (itVector = itMap->second.begin(); itVector != itMap->second.end(); itVector++)
        {
            nNodeCores = (*itVector)->getNumCores();
            nTotalCores += nNodeCores;
            nUsedCores += nNodeCores - (*itVector)->getAvailableCores();
        }
    }

    if (nTotalCores > 0)
        dPercentage = nUsedCores / (double)nTotalCores;

    return dPercentage;
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

std::string DataCentreManagerBase::dataCentreToString()
{
    // FIXME: What was this inteded for and why is it empty ?
    std::ostringstream info;
    int i;
    info << '\n';
    return info.str();
}

cGate *DataCentreManagerBase::getOutGate(cMessage *msg)
{
    // If msg arrives from the Hypervisor
    if (msg->getArrivalGate() == inGate)
    {
        return outGate;
    }
    // Msg arrives from an unknown gate
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
    EV_FATAL << "#___cpuUsage#" << strName << " " << simTime().dbl() << " " << getCurrentCpuPercentageUsage() << "\n";

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
    int machinesInUse = getMachinesInUse();
    int activeMachines = getActiveMachines();
    int activeOrInUseMachines = getActiveOrInUseMachines();
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
            setActiveMachines(machinesInUseForecasting + nActiveMachinesThreshold);
    }
}

void DataCentreManagerBase::manageActiveMachines()
{
    int machinesInUse, activeMachines;
    machinesInUse = getMachinesInUse();

    if (forecastActiveMachines)
    {
        activeMachines = getActiveMachines();
        if (machinesInUse + nActiveMachinesThreshold > activeMachines)
        {
            setActiveMachines(machinesInUse + nActiveMachinesThreshold);
        }
    }
    else if (!forecastActiveMachines and nActiveMachinesThreshold > 0)
    {
        setActiveMachines(machinesInUse + nActiveMachinesThreshold);
    }
}

std::tuple<unsigned int, simtime_t **, simtime_t *> DataCentreManagerBase::getTimersTupleByHypervisorPath(const std::string &fullPath)
{
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple = std::make_tuple(0, nullptr, nullptr);
    return getOrDefault(mapCpuUtilizationTimePerHypervisor, fullPath, timersTuple);
}

simtime_t **DataCentreManagerBase::getStartTimeByHypervisorPath(std::string strHypervisorFullPath)
{
    std::tuple<unsigned int, simtime_t **, simtime_t *> timersTuple = getTimersTupleByHypervisorPath(strHypervisorFullPath);
    unsigned int numCores = std::get<0>(timersTuple);
    simtime_t **startTimesArray = nullptr;

    if (numCores < 1)
        error("%s - Unable to update cpu utilization times. Wrong Hypervisor full path [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strHypervisorFullPath.c_str());

    startTimesArray = std::get<1>(timersTuple);

    return startTimesArray;
}

simtime_t *DataCentreManagerBase::getTimerArrayByHypervisorPath(std::string strHypervisorFullPath)
{
    std::tuple<unsigned int, simtime_t **, simtime_t *> timersTuple = getTimersTupleByHypervisorPath(strHypervisorFullPath);
    unsigned int numCores = std::get<0>(timersTuple);
    simtime_t *timerArray = nullptr;

    if (numCores < 1)
        error("%s - Unable to update cpu utilization times. Wrong Hypervisor full path [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strHypervisorFullPath.c_str());

    timerArray = std::get<2>(timersTuple);

    return timerArray;
}

void DataCentreManagerBase::createDummyAppInAppModule(cModule *pVmAppModule)
{
    std::string strAppType = "simcan2.Applications.UserApps.DummyApp.DummyApplication";
    cModuleType *moduleType = cModuleType::get(strAppType.c_str());

    if (pVmAppModule == nullptr)
        return;

    cModule *moduleApp = moduleType->create("app", pVmAppModule);
    moduleApp->finalizeParameters();

    pVmAppModule->gate("fromHub")->connectTo(moduleApp->gate("in"));
    moduleApp->gate("out")->connectTo(pVmAppModule->gate("toHub"));

    // create internals, and schedule it
    moduleApp->buildInside();
    moduleApp->scheduleStart(simTime());
    moduleApp->callInitialize();
}

void DataCentreManagerBase::cleanAppVectorModule(cModule *pVmAppVectorModule)
{
    cModule *pVmAppModule = nullptr;

    int numMaxApps = pVmAppVectorModule->par("numApps");

    for (int i = 0; i < numMaxApps; i++)
    {
        pVmAppModule = pVmAppVectorModule->getSubmodule("appModule", i);
        // Disconnect and delete dummy app
        appBuilder->deleteApp(pVmAppModule);
        createDummyAppInAppModule(pVmAppModule);
    }
}

void DataCentreManagerBase::storeAppFromModule(cModule *pVmAppModule)
{
    cModule *pDummyAppModule = pVmAppModule->getSubmodule("app");
    pVmAppModule->gate("fromHub")->disconnect();
    pVmAppModule->gate("toHub")->getPreviousGate()->disconnect();
    pDummyAppModule->changeParentTo(this);
    std::string appInstance = pDummyAppModule->par("appInstance");
    LocalApplication *ptrAppInstance = dynamic_cast<LocalApplication *>(pDummyAppModule);
    unsigned int *appStateArr = new unsigned int[2];
    appStateArr[0] = ptrAppInstance->getCurrentIteration();
    appStateArr[1] = ptrAppInstance->getCurrentRemainingMIs();
    // ptrAppInstance->sendAbortRequest();
    mapAppsModulePerId[appInstance] = appStateArr;
    pDummyAppModule->deleteModule();
}

cModule *DataCentreManagerBase::getFreeAppModuleInVector(const std::string &vmId)
{
    cModule *appVector = getAppsVectorModulePerVm(vmId);
    if (appVector == nullptr)
        error("[%s] There is no app ventor module for the VM!!", LogUtils::prettyFunc(__FILE__, __func__).c_str());

    int numMaxApps = appVector->par("numApps");
    bool *runningAppsArr = getAppsRunningInVectorModuleByVm(vmId);

    for (int i = 0; i < numMaxApps; i++)
    {
        if (!runningAppsArr[i])
        {
            runningAppsArr[i] = true;
            return appVector->getSubmodule("appModule", i);
        }
    }

    return nullptr;
}

void DataCentreManagerBase::handleUserAppRequest(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle AppRequest" << '\n';

    // Cast request
    auto userAPP_Rq = check_and_cast<SM_UserAPP *>(sm);
    std::string userId = userAPP_Rq->getUserID();

    // Sanity check
    if (userAPP_Rq->getAppArraySize() < 1)
        error("[%s] The list of applications of the user: %s is empty!!", LogUtils::prettyFunc(__FILE__, __func__).c_str(), userId.c_str());

    // FIXME: The behaviour for rejecting app requests is not defined!
    // Prepare context
    ApplicationBuilder::ApplicationContext ctx;
    bool bHandle = false;
    ctx.userId = &userId;

    // Print the request
    userAPP_Rq->printUserAPP();

    for (unsigned int i = 0; i < userAPP_Rq->getAppArraySize(); i++)
    {
        // Get the app
        APP_Request userApp = userAPP_Rq->getApp(i);

        if (userApp.eState == appFinishedOK || userApp.eState == appFinishedError)
            continue;

        // Fill the context
        ctx.appId = &userApp.strApp;
        ctx.vmId = &userApp.vmId;
        ctx.schema = dataManager->searchApp(userApp.strAppType);

        if (ctx.schema == nullptr)
            error("%s - Unable to find App. Wrong AppType [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), userApp.strAppType.c_str());

        // Locate the free "app slot"
        cModule *pVmAppModule = getFreeAppModuleInVector(userApp.vmId);
        if (pVmAppModule == nullptr)
            continue;

        // Build the application
        appBuilder->build(pVmAppModule, ctx);

        userAPP_Rq->changeState(userApp.strApp, userApp.vmId, appRunning);
    }
    bHandle = true;

    if (!bHandle)
    {
        rejectAppRequest(userAPP_Rq);
        return;
    }

    // Search for the app in manager map
    auto appIt = handlingAppsRqMap.find(userId);
    if (appIt == handlingAppsRqMap.end())
    {
        // Registering the appRq
        handlingAppsRqMap[userId] = userAPP_Rq;
    }
    else
    {
        SM_UserAPP *uapp = appIt->second;
        uapp->update(userAPP_Rq);
        delete userAPP_Rq; // Delete ephemeral message after update global message.
    }
}

void DataCentreManagerBase::rejectAppRequest(SM_UserAPP *userAPP_Rq)
{
    // Create a request_rsp message
    EV_INFO << "Rejecting app request from user:" << userAPP_Rq->getUserID() << '\n';

    // Fill the message
    userAPP_Rq->setIsResponse(true);
    userAPP_Rq->setOperation(SM_APP_Req);
    userAPP_Rq->setResult(SM_APP_Res_Reject);

    // Send the values
    sendResponseMessage(userAPP_Rq);
}

void DataCentreManagerBase::handleVmRequestFits(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << '\n';

    auto userVM_Rq = check_and_cast<SM_UserVM *>(sm);
    userVM_Rq->printUserVM();

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
    EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserID() << '\n';
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
    EV_INFO << "Notifying request from user: " << userVM_Rq->getUserID() << '\n';
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

void DataCentreManagerBase::handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndexInVector)
{
    SM_UserAPP *pUserApp;
    pUserApp = getUserAppRequestPerUser(strUsername);

    if (pUserApp == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app request message from the User!!!").c_str());

    //    endSingleAppResponse(pUserApp, strVmId, strAppName);
    //    scheduleAppTimeout(EXEC_APP_END_SINGLE, strUsername, strAppName, strVmId, SimTime());

    EV_INFO << "The execution of the App [" << strAppName << " / " << strVmId
            << "] launched by the user " << strUsername << " has finished" << '\n';

    pUserApp->increaseFinishedApps();
    // Check for a possible timeout
    if (!pUserApp->isFinishedKO(strAppName, strVmId))
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - Changing status of the application [ app: "
                << strAppName << " | vmId: " << strVmId << '\n';
        pUserApp->printUserAPP();

        pUserApp->changeState(strAppName, strVmId, appFinishedOK);
        pUserApp->setEndTime(strAppName, strVmId, simTime().dbl());
    }

    cModule *pVmAppVectorModule = nullptr;
    bool *runningAppsArr;

    pVmAppVectorModule = getAppsVectorModulePerVm(strVmId);

    if (pVmAppVectorModule == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app ventor module for the VM!!").c_str());

    int numMaxApps = pVmAppVectorModule->par("numApps");

    if (appIndexInVector >= numMaxApps)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app ventor module for the VM!!").c_str());

    runningAppsArr = getAppsRunningInVectorModuleByVm(strVmId);

    if (runningAppsArr == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app runing vector for the VM!!").c_str());

    runningAppsArr[appIndexInVector] = false;
}

void DataCentreManagerBase::acceptVmRequest(SM_UserVM *userVM_Rq)
{
    EV_INFO << "Accepting request from user:" << userVM_Rq->getUserID() << '\n';

    //    if(acceptedUsersRqMap.find(userVM_Rq->getUserID()) == acceptedUsersRqMap.end())
    //    {
    //        //Accepting new user
    //        EV_INFO << "Registering new user in the system:" << userVM_Rq->getUserID() << '\n';
    //        acceptedUsersRqMap[userVM_Rq->getUserID()] = userVM_Rq;
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

    EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserID() << '\n';

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
    int nRequestedVms = userVM_Rq->getTotalVmsRequests();
    std::string userId = userVM_Rq->getUserID();

    EV_DEBUG << "checkVmUserFit- Init" << '\n';
    EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userId << '\n';

    // Before starting the process, it is neccesary to check if the
    int nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
    int nAvailableCores = getNTotalAvailableCores();

    if (nTotalRequestedCores > nAvailableCores)
    {
        EV_DEBUG << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserID() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';
        return false;
    }

    int nTotalCores = getNTotalCores();
    EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserID() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';

    // Process all the VMs
    bool bRet, bAccepted;
    bAccepted = bRet = true;

    for (int i = 0; i < nRequestedVms && bRet; i++)
    {
        EV_DEBUG << "\ncheckVmUserFit - Trying to handle the VM: " << i << '\n';

        // Get the VM request
        auto vmRequest = userVM_Rq->getVms(i);

        Hypervisor *hypervisor = selectNode(userVM_Rq, vmRequest);

        if (hypervisor != nullptr)
        {
            acceptedVMsMap[vmRequest.strVmId] = hypervisor;
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
            int nRentTime;
            // Getting VM and scheduling renting timeout
            strVmId = userVM_Rq->getStrVmId();
            if (!strVmId.empty() && userVM_Rq->getOperation() == SM_VM_Sub)
                nRentTime = 3600;
            else
                nRentTime = vmRequest.nRentTime_t2;

            // vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.strVmType, nRentTime);
            vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, userId, vmRequest, nRentTime);
        }

        EV_DEBUG << "checkVmUserFit - Updated space#: [" << userVM_Rq->getUserID() << "Requested: " << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';
    }

    if (bRet)
    {
        acceptedUsersRqMap[userId] = userVM_Rq;
        nTotalAvailableCores -= nTotalRequestedCores;
        EV_DEBUG << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserID() << '\n';
    }
    else
    {
        EV_DEBUG << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserID() << '\n';
    }

    EV_DEBUG << "checkVmUserFit- End" << '\n';

    return bRet;
}

void DataCentreManagerBase::abortAllApps(std::string strVmId)
{
    cModule *pAppModule, *pVmAppModule, *pVmAppVectorModule = getAppsVectorModulePerVm(strVmId);
    bool *appRunningArr = getAppsRunningInVectorModuleByVm(strVmId);
    int numMaxApps = pVmAppVectorModule->par("numApps");

    for (int i = 0; i < numMaxApps; i++)
    {
        if (appRunningArr[i])
        {
            pVmAppModule = pVmAppVectorModule->getSubmodule("appModule", i);
            // deleteAppFromModule(pVmAppModule);
            storeAppFromModule(pVmAppModule);
            createDummyAppInAppModule(pVmAppModule);
            appRunningArr[i] = false;
        }
    }
}

void DataCentreManagerBase::updateCpuUtilizationTimeForHypervisor(Hypervisor *pHypervisor)
{
    unsigned int numCores = getNumCoresByHypervisorPath(pHypervisor->getFullPath());
    simtime_t **startTimesArray = getStartTimeByHypervisorPath(pHypervisor->getFullPath());
    simtime_t *timersArray = getTimerArrayByHypervisorPath(pHypervisor->getFullPath());
    bool *freeCoresArrayPtr = pHypervisor->getFreeCoresArrayPtr();

    for (int i = 0; i < numCores; i++)
    {
        if (freeCoresArrayPtr[i] && startTimesArray[i] != nullptr)
        {
            timersArray[i] += simTime() - *(startTimesArray[i]);
            delete startTimesArray[i];
            startTimesArray[i] = nullptr;
        }
        else if (!freeCoresArrayPtr[i] && startTimesArray[i] == nullptr)
        {
            startTimesArray[i] = new SimTime(simTime());
        }
    }
}

void DataCentreManagerBase::deallocateVmResources(std::string strVmId)
{
    Hypervisor *pHypervisor = getNodeHypervisorByVm(strVmId);

    if (pHypervisor == nullptr)
        error("%s - Unable to deallocate VM. Wrong VM name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strVmId.c_str());

    pHypervisor->deallocateVmResources(strVmId);

    updateCpuUtilizationTimeForHypervisor(pHypervisor);

    manageActiveMachines();
}

// TODO: asignar la vm que hace el timout al mensaje de la app. Duplicarlo y enviarlo.
void DataCentreManagerBase::handleExecVmRentTimeout(cMessage *msg)
{
    SM_UserAPP *pUserApp;
    Hypervisor *pHypervisor;

    std::string strUsername,
        strVmId,
        strVmType,
        strAppName,
        strIp;

    bool bAlreadyFinished;

    SM_UserVM_Finish *pUserVmFinish;

    std::map<std::string, SM_UserAPP *>::iterator it;

    pUserVmFinish = dynamic_cast<SM_UserVM_Finish *>(msg);
    if (pUserVmFinish == nullptr)
        error("%s - Unable to cast msg to SM_UserVM_Finish*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), msg->getName());

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - INIT" << '\n';
    strVmId = pUserVmFinish->getStrVmId();
    strVmType = pUserVmFinish->getStrVmType();

    strUsername = pUserVmFinish->getUserID();
    EV_INFO << "The rent of the VM [" << strVmId
            << "] launched by the user " << strUsername
            << " has finished" << '\n';

    deallocateVmResources(strVmId);
    nTotalAvailableCores += getTotalCoresByVmType(strVmType);

    pUserApp = getUserAppRequestPerUser(strUsername);

    if (pUserApp == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app request message from the User!!").c_str());

    //    acceptAppRequest(pUserApp, strVmId);

    // Check the Application status

    EV_INFO << "Last id gate: " << pUserApp->getLastGateId() << '\n';
    EV_INFO << "Checking the status of the applications which are running over this VM\n";

    // Abort the running applications
    if (!pUserApp->allAppsFinished(strVmId))
    {
        EV_INFO << "Aborting running applications\n";
        abortAllApps(strVmId);
        pUserApp->abortAllApps(strVmId);
    }
    // Check the result and send it
    checkAllAppsFinished(pUserApp, strVmId);

    EV_INFO << "Freeing resources...\n";

    // Delete the event!
    delete msg;
}

void DataCentreManagerBase::checkAllAppsFinished(SM_UserAPP *pUserApp, std::string strVmId)
{
    assert_msg(pUserApp != nullptr, "Nullpointer in argument detected!");

    auto userId = pUserApp->getUserID();

    if (!pUserApp->allAppsFinished(strVmId))
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - Total apps finished: "
                << pUserApp->getNFinishedApps() << " of "
                << pUserApp->getAppArraySize() << '\n';
        return;
    }

    if (pUserApp->allAppsFinishedOK(strVmId))
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - All the apps corresponding with the user "
                << userId
                << " have finished successfully" << '\n';

        pUserApp->printUserAPP();

        // Notify the user the end of the execution
        acceptAppRequest(pUserApp, strVmId);
    }
    else
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - All the apps corresponding with the user "
                << userId
                << " have finished with some errors" << '\n';

        // Check the subscription queue
        // updateSubsQueue();

        // if (!pUserApp->getFinished())
        timeoutAppRequest(pUserApp, strVmId); // Notify the user the end of the execution
    }

    // Delete the application on the hashmap
    // handlingAppsRqMap.erase(userId);
}

void DataCentreManagerBase::acceptAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId)
{
    EV_INFO << "Sending vm end to the CP:" << userAPP_Rq->getUserID() << '\n';

    SM_UserAPP *userAPP_Res = userAPP_Rq->dup();
    userAPP_Res->printUserAPP();

    userAPP_Res->setVmId(strVmId.c_str());
    userAPP_Res->setFinished(true);

    // Fill the message
    userAPP_Res->setIsResponse(true);
    userAPP_Res->setOperation(SM_APP_Rsp);
    userAPP_Res->setResult(SM_APP_Res_Accept);

    // Send the values
    sendResponseMessage(userAPP_Res);
}

void DataCentreManagerBase::timeoutAppRequest(SM_UserAPP *userAPP_Rq, std::string strVmId)
{
    EV_INFO << "Sending timeout to the user:" << userAPP_Rq->getUserID() << '\n';
    EV_INFO << "Last id gate: " << userAPP_Rq->getLastGateId() << '\n';

    SM_UserAPP *userAPP_Res = userAPP_Rq->dup(strVmId);
    userAPP_Res->printUserAPP();

    userAPP_Res->setVmId(strVmId.c_str());

    // Fill the message
    userAPP_Res->setIsResponse(true);
    userAPP_Res->setOperation(SM_APP_Rsp);
    userAPP_Res->setResult(SM_APP_Res_Timeout);

    // Send the values
    sendResponseMessage(userAPP_Res);
}

void DataCentreManagerBase::clearVMReq(SM_UserVM *&userVM_Rq, int lastId)
{
    for (int i = 0; i < lastId; i++)
    {
        auto vmRequest = userVM_Rq->getVms(i);
        cancelAndDelete(vmRequest.pMsg);
        vmRequest.pMsg = nullptr;

        deallocateVmResources(vmRequest.strVmId);

        // datacentreCollection->freeVmRequest(vmRequest.strVmId);
    }
}

bool DataCentreManagerBase::allocateVM(NodeResourceRequest *pResourceRequest, Hypervisor *pHypervisor)
{
    // TODO: Probablemente sea mejor mover esto al hypervisor. La asignaci�n al map y que sea el hypervisor el que controle a que VM va.
    // TODO: Finalmente deber�a devolver la IP del nodo y que el mensaje de la App llegue al nodo.
    cModule *pVmAppVectorModule = pHypervisor->allocateNewResources(pResourceRequest);
    if (pVmAppVectorModule != nullptr)
    {
        updateCpuUtilizationTimeForHypervisor(pHypervisor);
        mapAppsVectorModulePerVm[pResourceRequest->getVmId()] = pVmAppVectorModule;
        int numMaxApps = pVmAppVectorModule->par("numApps");
        bool *runningAppsArr = new bool[numMaxApps]{false};
        mapAppsRunningInVectorModulePerVm[pResourceRequest->getVmId()] = runningAppsArr;
        return true;
    }
    return false;
}

NodeResourceRequest *DataCentreManagerBase::generateNode(std::string strUserName, VM_Request vmRequest)
{
    NodeResourceRequest *pNode = new NodeResourceRequest();

    pNode->setUserName(strUserName);
    pNode->setMaxStartTimeT1(vmRequest.maxStartTime_t1);
    pNode->setRentTimeT2(vmRequest.nRentTime_t2);
    pNode->setMaxSubTimeT3(vmRequest.maxSubTime_t3);
    pNode->setMaxSubscriptionTimeT4(vmRequest.maxSubscriptionTime_t4);
    pNode->setVmId(vmRequest.strVmId);
    fillVmFeatures(vmRequest.strVmType, pNode);

    return pNode;
}

void DataCentreManagerBase::fillVmFeatures(std::string strVmType, NodeResourceRequest *&pNode)
{
    auto pVmType = dataManager->searchVirtualMachine(strVmType);

    if (pVmType != nullptr)
    {
        EV_INFO << "fillVmFeatures - Vm:" << strVmType << " cpus: " << pVmType->getNumCores() << " mem: " << pVmType->getMemoryGb() << '\n';

        pNode->setTotalCpUs(pVmType->getNumCores());
        pNode->setTotalMemory(pVmType->getMemoryGb());
        pNode->setTotalDiskGb(pVmType->getDiskGb());
    }
}

void DataCentreManagerBase::printFinal()
{
    simtime_t finalSimulationTime = simTime();
    const char *strName = getParentModule()->getName();
    int nTotalIndex = 0;

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
}

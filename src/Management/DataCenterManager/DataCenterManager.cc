#include "DataCenterManager.h"
#include "Management/utils/LogUtils.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"


Define_Module(DataCenterManager);

DataCenterManager::~DataCenterManager(){
    acceptedVMsMap.clear();
    handlingAppsRqMap.clear();
    mapHypervisorPerNodes.clear();
    mapAppsVectorModulePerVm.clear();
    mapAppsModulePerId.clear();
    mapAppsRunningInVectorModulePerVm.clear();
    mapCpuUtilizationTimePerHypervisor.clear();
}

void DataCenterManager::initialize(){

    int result;

    // Init super-class
    CloudManagerBase::initialize();

        // Get parameters from module
        showDataCenterConfig = par ("showDataCenterConfig");
        nCpuStatusInterval = par ("cpuStatusInterval");

        // Get gates
        inGate = gate ("in");
        outGate = gate ("out");

        // Parse data-center list
        result = parseDataCenterConfig();
        result = initDataCenterMetadata();

        // Something goes wrong...
        if (result == SC_ERROR){
            error ("Error while parsing data-centers list");
        }
        else if (showDataCenterConfig){
            EV_DEBUG << dataCenterToString ();
        }

        cpuStatusMessage = new cMessage(CPU_STATUS);
        scheduleAt(SimTime(), cpuStatusMessage);
}

int DataCenterManager::initDataCenterMetadata (){
    int result = SC_OK;
    DataCenter *pDataCenterBase = dataCentersBase[0];
    cModule *pRackModule, *pBoardModule, *pNodeModule;
    int numBoards, numNodes, numCores, numTotalCores = 0;

    for (int nRackIndex=0; nRackIndex < pDataCenterBase->getNumRacks(false); nRackIndex++ ) {
        Rack* pRackBase = pDataCenterBase->getRack(nRackIndex,false);
        //Generate rack name in the data center
        string strRackName = "rackCmp_" + pRackBase->getRackInfo()->getName();
        pRackModule = getParentModule()->getSubmodule(strRackName.c_str(), nRackIndex);

        numBoards =  pRackModule->par("numBoards");

        for (int nBoardIndex=0; nBoardIndex < numBoards; nBoardIndex++) {
            pBoardModule = pRackModule->getSubmodule("board", nBoardIndex);
            numNodes = pBoardModule->par("numBlades");
            numTotalCores+=numNodes;

            for (int nNodeIndex=0; nNodeIndex < numNodes; nNodeIndex++) {
                pNodeModule = pBoardModule->getSubmodule("blade", nNodeIndex);

                storeNodeMetadata(pNodeModule);

            }
        }


    }

    nTotalCores = nTotalAvailableCores = numTotalCores;

    return result;
}

int DataCenterManager::storeNodeMetadata(cModule *pNodeModule) {
    int result = SC_OK;
    cModule *pHypervisorModule;
    Hypervisor *pHypervisor;
    int numCores;

    pHypervisorModule = pNodeModule->getSubmodule("osModule")->getSubmodule("hypervisor");

    numCores = pNodeModule->par("numCpuCores");

    pHypervisor = check_and_cast<Hypervisor*>(pHypervisorModule);

    simtime_t **startTimeArray = new simtime_t *[numCores];
    simtime_t *timerArray = new simtime_t[numCores];
    for (int i=0; i<numCores;i++) {
        startTimeArray[i] = nullptr;
        timerArray[i] = SimTime();
    }


    //Store hypervisor pointers by number of cores
    mapHypervisorPerNodes[numCores].push_back(pHypervisor);
    //Initialize cpu utilization timers
    mapCpuUtilizationTimePerHypervisor[pHypervisorModule->getFullPath()] = std::make_tuple(numCores, startTimeArray, timerArray);

    return result;
}

double DataCenterManager::getCurrentCpuPercentageUsage() {
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;
    std::vector<Hypervisor*>::iterator itVector;
    int nTotalCores, nUsedCores, nNodeCores;
    double dPercentage = 0.0;

    nTotalCores = nUsedCores = 0;

    for (itMap=mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end(); itMap++){
        nNodeCores = itMap->first;
        for (itVector=itMap->second.begin(); itVector != itMap->second.end(); itVector++) {
            nTotalCores += nNodeCores;
            nUsedCores += nNodeCores - (*itVector)->getAvailableCores();
        }
    }

    if (nTotalCores > 0)
        dPercentage = nUsedCores / (double)nTotalCores;

    return dPercentage;
}


int DataCenterManager::parseDataCenterConfig (){
    int result;
    const char *dataCentersConfigChr;

    dataCentersConfigChr= par ("dataCenterConfig");
    DataCenterConfigParser dataCenterParser(dataCentersConfigChr);
    result = dataCenterParser.parse();
    if (result == SC_OK) {
        dataCentersBase = dataCenterParser.getResult();
    }
    return result;
}

void DataCenterManager::parseConfig(){
    CloudManagerBase::parseConfig();
    parseDataCenterConfig();
}


std::string DataCenterManager::dataCenterToString (){

    std::ostringstream info;
    int i;

        info << std::endl;

    return info.str();
}


cGate* DataCenterManager::getOutGate (cMessage *msg){

    cGate* nextGate;

        // Init...
        nextGate = nullptr;

        // If msg arrives from the Hypervisor
        if (msg->getArrivalGate()==inGate){
            nextGate = outGate;
        }

        // Msg arrives from an unknown gate
        else
            error ("Message received from an unknown gate [%s]", msg->getName());


    return nextGate;
}

/**
 * This method initializes the self message handlers
 */
void DataCenterManager::initializeSelfHandlers() {
    // ADAA
//    selfHandlers[INITIAL_STAGE] = [this](cMessage *msg) -> void { return handleInitialStage(msg); };
    selfHandlers[EXEC_VM_RENT_TIMEOUT] = [this](cMessage *msg) -> void { return handleExecVmRentTimeout(msg); };
    //selfHandlers[EXEC_APP_END] = [this](cMessage *msg) -> void { return handleAppExecEnd(msg); };
//    selfHandlers[EXEC_APP_END_SINGLE] = [this](cMessage *msg) -> void { return handleAppExecEndSingle(msg); };
//    selfHandlers[USER_SUBSCRIPTION_TIMEOUT] = [this](cMessage *msg) -> void { return handleSubscriptionTimeout(msg); };
//    selfHandlers[MANAGE_SUBSCRIBTIONS] = [this](cMessage *msg) -> void { return handleManageSubscriptions(msg); };
    selfHandlers[CPU_STATUS] = [this](cMessage *msg) -> void { return handleCpuStatus(msg); };
}

/**
 * This method initializes the request handlers
 */
void DataCenterManager::initializeRequestHandlers() {
    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void { return handleVmRequestFits(msg); };
    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void { return handleVmSubscription(msg); };
    requestHandlers[SM_APP_Req] = [this](SIMCAN_Message *msg) -> void { return handleUserAppRequest(msg); };
}

void DataCenterManager::processResponseMessage (SIMCAN_Message *sm){
    error ("This module cannot process response messages:%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
}

void DataCenterManager::handleCpuStatus(cMessage *msg ) {
    const char *strName = getParentModule()->getName();
    EV_FATAL << "#___cpuUsage#" << strName << " " << simTime().dbl() << " " << getCurrentCpuPercentageUsage() << endl;
    cpuStatusMessage = nullptr;
    if (!bFinished) {
        cpuStatusMessage = new cMessage(CPU_STATUS);
        scheduleAt(simTime() + SimTime(nCpuStatusInterval, SIMTIME_S), cpuStatusMessage);
    }

}

Application* DataCenterManager::searchAppPerType(std::string strAppType)
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

SM_UserAPP* DataCenterManager::getUserAppRequestPerUser(std::string strUserId) {
    SM_UserAPP *pUserApp = nullptr;
    std::map<std::string, SM_UserAPP*>::iterator it;
    it = handlingAppsRqMap.find(strUserId);
    if (it != handlingAppsRqMap.end())
    {
        pUserApp = it->second;
    }
    return pUserApp;
}

cModule* DataCenterManager::getAppsVectorModulePerVm(std::string strVmId) {
    cModule *pVmAppVectorModule = nullptr;
    std::map<std::string, cModule*>::iterator itAppVectorModule;
    itAppVectorModule = mapAppsVectorModulePerVm.find(strVmId);
    if (itAppVectorModule != mapAppsVectorModulePerVm.end())
    {
        pVmAppVectorModule = itAppVectorModule->second;
    }
    return pVmAppVectorModule;
}

cModule* DataCenterManager::getFreeAppModuleInVector(std::string strVmId) {
    cModule *pVmAppModule = nullptr;
    cModule *pVmAppVectorModule = nullptr;
    bool bFound = false;
    bool *runningAppsArr;

    pVmAppVectorModule = getAppsVectorModulePerVm(strVmId);

    if (pVmAppVectorModule == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app ventor module for the VM!!").c_str());

    int numMaxApps = pVmAppVectorModule->par("numApps");

    runningAppsArr = getAppsRunningInVectorModuleByVm(strVmId);

    for (int i=0; i < numMaxApps && !bFound; i++) {
        if (!runningAppsArr[i]){
            runningAppsArr[i] = true;
            pVmAppModule = pVmAppVectorModule->getSubmodule("appModule", i);
            bFound = true;
        }
    }

    return pVmAppModule;
}


std::tuple<unsigned int, simtime_t**, simtime_t*> DataCenterManager::getTimersTupleByHypervisorPath(std::string strHypervisorFullPath) {
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple = std::make_tuple(0, nullptr, nullptr);
    std::map<std::string, std::tuple<unsigned int, simtime_t**, simtime_t*>>::iterator it;

    it = mapCpuUtilizationTimePerHypervisor.find(strHypervisorFullPath);
    if (it != mapCpuUtilizationTimePerHypervisor.end()) {
        timersTuple = it->second;
    }
    return timersTuple;
}

simtime_t** DataCenterManager::getStartTimeByHypervisorPath(std::string strHypervisorFullPath) {
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple = getTimersTupleByHypervisorPath(strHypervisorFullPath);
    unsigned int numCores = std::get<0>(timersTuple);
    simtime_t **startTimesArray = nullptr;

    if (numCores < 1)
        error ("%s - Unable to update cpu utilization times. Wrong Hypervisor full path [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strHypervisorFullPath.c_str());

    startTimesArray = std::get<1>(timersTuple);

    return startTimesArray;
}

simtime_t* DataCenterManager::getTimerArrayByHypervisorPath(std::string strHypervisorFullPath) {
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple = getTimersTupleByHypervisorPath(strHypervisorFullPath);
    unsigned int numCores = std::get<0>(timersTuple);
    simtime_t *timerArray = nullptr;

    if (numCores < 1)
        error ("%s - Unable to update cpu utilization times. Wrong Hypervisor full path [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strHypervisorFullPath.c_str());

    timerArray = std::get<2>(timersTuple);

    return timerArray;
}

unsigned int DataCenterManager::getNumCoresByHypervisorPath(std::string strHypervisorFullPath) {
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple = getTimersTupleByHypervisorPath(strHypervisorFullPath);
    unsigned int numCores = std::get<0>(timersTuple);

    return numCores;
}

Hypervisor* DataCenterManager::getNodeHypervisorByVm(std::string strVmId) {
    Hypervisor* pHypervisor = nullptr;
    std::map<std::string, Hypervisor*>::iterator it;

    it = acceptedVMsMap.find(strVmId);
    if (it != acceptedVMsMap.end()) {
        pHypervisor = it->second;

    }
    return pHypervisor;
}

bool* DataCenterManager::getAppsRunningInVectorModuleByVm(std::string strVmId) {
    bool* appsRunningArr = nullptr;
    std::map<std::string, bool*>::iterator it;

    it = mapAppsRunningInVectorModulePerVm.find(strVmId);
    if (it != mapAppsRunningInVectorModulePerVm.end()) {
        appsRunningArr = it->second;

    }
    return appsRunningArr;
}

unsigned int* DataCenterManager::getAppModuleById(std::string appInstance) {
    unsigned int* appModule = nullptr;
    std::map<std::string, unsigned int*>::iterator it;

    it = mapAppsModulePerId.find(appInstance);
    if (it != mapAppsModulePerId.end()) {
        appModule = it->second;

    }
    return appModule;
}


void DataCenterManager::createDummyAppInAppModule(cModule *pVmAppModule) {
    std::string strAppType = "simcan2.Applications.UserApps.DummyApp.DummyApplication";
    cModuleType *moduleType = cModuleType::get(strAppType.c_str());

    if (pVmAppModule==nullptr)
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


void DataCenterManager::cleanAppVectorModule(cModule *pVmAppVectorModule){
    cModule *pVmAppModule = nullptr;

    int numMaxApps = pVmAppVectorModule->par("numApps");

    for (int i=0; i < numMaxApps; i++) {
        pVmAppModule = pVmAppVectorModule->getSubmodule("appModule", i);
        // Disconnect and delete dummy app
        deleteAppFromModule(pVmAppModule);
        createDummyAppInAppModule(pVmAppModule);
    }
}

void DataCenterManager::storeAppFromModule(cModule *pVmAppModule) {
    cModule *pDummyAppModule = pVmAppModule->getSubmodule("app");
    pVmAppModule->gate("fromHub")->disconnect();
    pVmAppModule->gate("toHub")->getPreviousGate()->disconnect();
    pDummyAppModule->changeParentTo(this);
    std::string appInstance = pDummyAppModule->par("appInstance");
    LocalApplication *ptrAppInstance = dynamic_cast<LocalApplication*>(pDummyAppModule);
    unsigned int *appStateArr = new unsigned int[2];
    appStateArr[0] = ptrAppInstance->getCurrentIteration();
    appStateArr[1] = ptrAppInstance->getCurrentRemainingMIs();
    //ptrAppInstance->sendAbortRequest();
    mapAppsModulePerId[appInstance] = appStateArr;
    pDummyAppModule->deleteModule();
}

void DataCenterManager::deleteAppFromModule(cModule *pVmAppModule) {
    cModule *pDummyAppModule = pVmAppModule->getSubmodule("app");
    pDummyAppModule->callFinish();
    pVmAppModule->gate("fromHub")->disconnect();
    pVmAppModule->gate("toHub")->getPreviousGate()->disconnect();
    pDummyAppModule->deleteModule();
}

void DataCenterManager::handleUserAppRequest(SIMCAN_Message *sm)
{
    //Get the user name, and recover the info about the VmRequests;
    SM_UserAPP *userAPP_Rq;
    cModule *pVmAppVectorModule;

    bool bHandle;

    std::string strUsername,
                strVmId,
                strIp,
                strAppName;

    SimTime appStartTime,
            appExecutedTime,
            nTotalTime;

    Application *appType;

    VM_Request vmRequest;

    APP_Request userApp;

    std::map<std::string, SM_UserVM*>::iterator it;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle AppRequest"  << endl;
    userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);

    if(userAPP_Rq == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userAPP_Rq. Nullpointer!!").c_str());

    bHandle = false;
    userAPP_Rq->printUserAPP();

    strUsername = userAPP_Rq->getUserID();

    if(userAPP_Rq->getArrayAppsSize() < 1)
      {
        EV_INFO << "WARNING! [" << LogUtils::prettyFunc(__FILE__, __func__) << "] The user: " << strUsername << "has the application list empty!"<< endl;
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] The list of applications of a the user is empty!!").c_str());
      }


    for(unsigned int i = 0; i < userAPP_Rq->getAppArraySize(); i++)
      {
        //Get the app
        userApp = userAPP_Rq->getApp(i);

        if (userApp.eState == appFinishedOK || userApp.eState == appFinishedError)
            continue;

        std::string strInputDataSize, strOutputDataSize;
        int nInputDataSize, nOutputDataSize, nMIs, nIterations, nTotalTime;
        AppParameter* paramInputDataSize, *paramOutputDataSize, *paramMIs, *paramIterations;

        appType = searchAppPerType(userApp.strAppType);

        if (appType == nullptr)
            error ("%s - Unable to find App. Wrong AppType [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), appType);


        //TODO: Cuidado con esto a ver si no peta.
        //Esto es un apaÃ±o temporal para no ejecutarlo en los datacenters reales
        if((appType->getAppName().compare("AppDataIntensive")==0) || (appType->getAppName().compare("AppCPUIntensive")==0))
        {
            //DatasetInput
            paramInputDataSize = appType->getParameterByName("inputDataSize");
            paramOutputDataSize = appType->getParameterByName("outputDataSize");
            paramMIs = appType->getParameterByName("MIs");
            paramIterations = appType->getParameterByName("iterations");

            if(paramInputDataSize != nullptr)
            {
                nInputDataSize = std::stoi(paramInputDataSize->getValue());
            }
            if(paramOutputDataSize != nullptr)
            {
                nOutputDataSize = std::stoi(paramOutputDataSize->getValue());
            }
            if(paramMIs != nullptr)
            {
                nMIs = std::stoi(paramMIs->getValue());
            }
            if(paramIterations != nullptr)
            {
                nIterations = std::stoi(paramIterations->getValue());
            }


           std::string strAppType = "simcan2.Applications.UserApps." + appType->getType() + "." +  appType->getType();
           cModuleType *moduleType = cModuleType::get(strAppType.c_str());

          cModule *pVmAppModule = getFreeAppModuleInVector(userApp.vmId);

           if (pVmAppModule==nullptr)
               continue;

           // Disconnect and delete dummy app
           deleteAppFromModule(pVmAppModule);

           cModule *moduleApp;

           moduleApp = moduleType->create("app", pVmAppModule);
          moduleApp->par("appInstance") = userApp.strApp;
          moduleApp->par("vmInstance") = userApp.vmId;
          moduleApp->par("userInstance") = strUsername;
          moduleApp->par("inputDataSize") = nInputDataSize;
          moduleApp->par("outputDataSize") = nOutputDataSize;
          moduleApp->par("MIs") = nMIs;
          moduleApp->par("iterations") = nIterations;

           unsigned int *appStateArr = getAppModuleById(userApp.strApp);

           if (appStateArr !=nullptr) {
               LocalApplication *appInstancePtr = dynamic_cast<LocalApplication*>(moduleApp);
               appInstancePtr->setCurrentIteration(appStateArr[0]);
               appInstancePtr->setCurrentRemainingMIs(appStateArr[1]);
               mapAppsModulePerId.erase(userApp.strApp);
               moduleApp->par("initialized") = true;
           }

           moduleApp->finalizeParameters();

           pVmAppModule->gate("fromHub")->connectTo(moduleApp->gate("in"));

           moduleApp->gate("out")->connectTo(pVmAppModule->gate("toHub"));

           // create internals, and schedule it
           moduleApp->buildInside();
           moduleApp->scheduleStart(simTime());

           moduleApp->callInitialize();

           userAPP_Rq->changeState(userApp.strApp, userApp.vmId, appRunning);
//           userAPP_Rq->changeStateByIndex(i, userApp.strApp, appRunning);



        }
        else if(appType!=NULL && appType->getAppName().compare("otraApp"))
        {

        }
      }
    bHandle = true;

    if (bHandle) {
        std::map<std::string, SM_UserAPP*>::iterator appIt = handlingAppsRqMap.find(strUsername);
        if(appIt == handlingAppsRqMap.end())
          {
            //Registering the appRq
            handlingAppsRqMap[strUsername] = userAPP_Rq;
          }
        else
          {
            SM_UserAPP *uapp = appIt->second;
            uapp->update(userAPP_Rq);
            delete userAPP_Rq; //Delete ephemeral message after update global message.
          }
    }

}

//void DataCenterManager::checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId) {
//    std::string strUsername;
//
//    if (pUserApp != nullptr)
//      {
//        strUsername = pUserApp->getUserID();
//        if (pUserApp->allAppsFinished(strVmId))
//          {
//            if (pUserApp->allAppsFinishedOK(strVmId))
//              {
//                EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
//                << " - All the apps corresponding with the user "
//                << strUsername
//                << " have finished successfully" << endl;
//
//                pUserApp->printUserAPP();
//
//                //Notify the user the end of the execution
//                acceptAppRequest(pUserApp, strVmId);
//
//              }
//            else
//              {
//                EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
//                << " - All the apps corresponding with the user "
//                << strUsername
//                << " have finished with some errors" << endl;
//
//                //Check the subscription queue
//                //updateSubsQueue();
//
//                //if (!pUserApp->getFinished())
//                timeoutAppRequest(pUserApp, strVmId);  //Notify the user the end of the execution
//              }
//
//            //Delete the application on the hashmap
//            //handlingAppsRqMap.erase(strUsername);
//          }
//        else
//          {
//            EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
//                    << " - Total apps finished: "
//                    << pUserApp->getNFinishedApps() << " of "
//                    << pUserApp->getAppArraySize() << endl;
//          }
//      }
//    else
//      {
//        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
//        << " - WARNING! Null pointer parameter "
//        << strUsername << endl;
//      }
//}

void DataCenterManager::handleVmRequestFits(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request"  << endl;

    if(userVM_Rq != nullptr)
    {
        userVM_Rq->printUserVM();
        //Check if is a VmRequest or a subscribe
        if (checkVmUserFit(userVM_Rq)) {
            EV_FATAL << "Ok" << endl;
            acceptVmRequest(userVM_Rq);
        }else{
            rejectVmRequest(userVM_Rq);
        }
    }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());
      }
}

void DataCenterManager::handleVmSubscription(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Subscribe operation"  << endl;

    if(userVM_Rq != nullptr)
      {
        if (checkVmUserFit(userVM_Rq))
            notifySubscription(userVM_Rq);
        else
            rejectVmSubscribe(userVM_Rq); //Store the vmRequest
      }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());
      }
}

void DataCenterManager::rejectVmSubscribe(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << endl;

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Reject);

    //Send the values
    sendResponseMessage(userVM_Rq);
}

void DataCenterManager::notifySubscription(SM_UserVM* userVM_Rq)
{
    SM_UserVM_Finish* pMsgTimeout;
    EV_INFO << "Notifying request from user: " << userVM_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << endl;

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Accept);

    //Cancel the timeout event
//    pMsgTimeout = userVM_Rq->getTimeoutSubscribeMsg();
//    if(pMsgTimeout != nullptr)
//    {
//        cancelAndDelete(pMsgTimeout);
//        userVM_Rq->setTimeoutSubscribeMsg(nullptr);
//    }

    //Send the values
    sendResponseMessage(userVM_Rq);
}

void DataCenterManager::handleAppExecEndSingle(std::string strUsername, std::string strVmId, std::string strAppName, int appIndexInVector) {
    SM_UserAPP *pUserApp;
    pUserApp = getUserAppRequestPerUser(strUsername);

    if (pUserApp == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app request message from the User!!").c_str());

//    endSingleAppResponse(pUserApp, strVmId, strAppName);
//    scheduleAppTimeout(EXEC_APP_END_SINGLE, strUsername, strAppName, strVmId, SimTime());

    EV_INFO << "The execution of the App [" << strAppName
                           << " / " << strVmId
                           << "] launched by the user " << strUsername
                           << " has finished" << endl;

    pUserApp->increaseFinishedApps();
    //Check for a possible timeout
    if (!pUserApp->isFinishedKO(strAppName, strVmId))
      {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
        << " - Changing status of the application [ app: "
        << strAppName << " | vmId: " << strVmId << endl;
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



void  DataCenterManager::acceptVmRequest(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Accepting request from user:" << userVM_Rq->getUserID() << endl;

//    if(acceptedUsersRqMap.find(userVM_Rq->getUserID()) == acceptedUsersRqMap.end())
//    {
//        //Accepting new user
//        EV_INFO << "Registering new user in the system:" << userVM_Rq->getUserID() << endl;
//        acceptedUsersRqMap[userVM_Rq->getUserID()] = userVM_Rq;
//    }

    //Fill the message
    //userVM_Rq->setIsAccept(true);
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Accept);

    //Send the values
    sendResponseMessage(userVM_Rq);

}

void  DataCenterManager::rejectVmRequest(SM_UserVM* userVM_Rq)
{
    //Create a request_rsp message

    EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserID() << endl;

    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Reject);

    //Send the values
    sendResponseMessage(userVM_Rq);
}

int DataCenterManager::getNTotalAvailableCores() const {
return nTotalAvailableCores;
}

void DataCenterManager::setNTotalAvailableCores(int nTotalAvailableCores) {
this->nTotalAvailableCores = nTotalAvailableCores;
}

int DataCenterManager::getNTotalCores() const {
return nTotalCores;
}

void DataCenterManager::setNTotalCores(int nTotalCores) {
this->nTotalCores = nTotalCores;
}

bool DataCenterManager::checkVmUserFit(SM_UserVM*& userVM_Rq)
{
    bool bRet,
         bAccepted;

    int nTotalRequestedCores,
        nRequestedVms,
        nAvailableCores,
        nTotalCores;

    std::string nodeIp,
                strUserName,
                strVmId;

    Hypervisor *hypervisor;

    bAccepted = bRet = true;
    if(userVM_Rq != nullptr)
      {
        nRequestedVms = userVM_Rq->getTotalVmsRequests();

        EV_DEBUG << "checkVmUserFit- Init" << endl;
        EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userVM_Rq->getUserID() << endl;

        //Before starting the process, it is neccesary to check if the
        nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
        nAvailableCores = getNTotalAvailableCores();

        if(nTotalRequestedCores<=nAvailableCores)
          {
            nTotalCores = getNTotalCores();
            EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserID() << nTotalRequestedCores<< " vs Available ["<< nAvailableCores << "/" <<nTotalCores << "]"<<endl;

            strUserName = userVM_Rq->getUserID();
            if (strUserName.compare("(0)User_A[65/100]")==0)
                EV_DEBUG << endl <<"Parar: " << endl;
            //Process all the VMs
            for(int i=0;i<nRequestedVms && bRet;i++)
              {
                EV_DEBUG << endl <<"checkVmUserFit - Trying to handle the VM: " << i << endl;

                //Get the VM request
                VM_Request& vmRequest = userVM_Rq->getVms(i);

//                //Create and fill the noderesource  with the VMrequest
//                NodeResourceRequest *pNode = generateNode(strUserName, vmRequest);
//
//                //Send the request to the DC
//                bAccepted = datacenterCollection->handleVmRequest(pNode);

                hypervisor = selectNode(strUserName, vmRequest);

                if (hypervisor != nullptr) {
                    acceptedVMsMap[vmRequest.strVmId] = hypervisor;
//                    userVM_Rq->createResponse(i, bAccepted, pNode->getStartTime(), pNode->getIp(), pNode->getPrice());

                    bAccepted = true;
                } else {
                    bAccepted = false;
                }

                bRet &= bAccepted;

                userVM_Rq->createResponse(i, bRet, simTime().dbl(), hypervisor->getFullPath(), 0);


                //We need to know the price of the Node.
                //userVM_Rq->createResponse(i, bAccepted, pNode->getStartTime(), pNode->getIp(), pNode->getPrice());
                //bRet &= bAccepted;

                if(!bRet)
                  {
                    clearVMReq (userVM_Rq, i);
                    //EV_DEBUG << "checkVmUserFit - The VM: " << i << "has not been handled, not enough space, all the request of the user " << strUserName << "have been deleted" << endl;
                  }
                else
                  {
                    //Getting VM and scheduling renting timeout
                    vmRequest.pMsg = scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.nRentTime_t2);

                    //Update value
                    //nAvailableCores = datacenterCollection->getTotalAvailableCores();
                    //EV_DEBUG << "checkVmUserFit - The VM: " << i << " has been handled and stored sucessfully, available cores: "<< nAvailableCores << endl;
                  }
              }
            //Update the data
            //nAvailableCores = datacenterCollection->getTotalAvailableCores();
            //nTotalCores = datacenterCollection->getTotalCores();

            EV_DEBUG << "checkVmUserFit - Updated space#: [" << userVM_Rq->getUserID() << "Requested: "<< nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
          }
        else
          {
            EV_DEBUG << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserID() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
            bRet = false;
          }

        if(bRet)
            EV_DEBUG << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserID() << endl;
        else
            EV_DEBUG << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserID() << endl;
      }
    else
      {
        EV_ERROR << "checkVmUserFit - WARNING!! nullpointer detected" <<endl;
        bRet = false;
      }

    EV_DEBUG << "checkVmUserFit- End" << endl;

    return bRet;
}

void DataCenterManager::abortAllApps(std::string strVmId)
{
    cModule *pAppModule, *pVmAppModule, *pVmAppVectorModule = getAppsVectorModulePerVm(strVmId);
    bool *appRunningArr = getAppsRunningInVectorModuleByVm(strVmId);
    int numMaxApps = pVmAppVectorModule->par("numApps");

    for (int i=0; i < numMaxApps; i++) {
        if (appRunningArr[i]){
            pVmAppModule = pVmAppVectorModule->getSubmodule("appModule", i);
            //deleteAppFromModule(pVmAppModule);
            storeAppFromModule(pVmAppModule);
            createDummyAppInAppModule(pVmAppModule);
            appRunningArr[i] = false;
        }
    }

}

void DataCenterManager::updateCpuUtilizationTimeForHypervisor(Hypervisor *pHypervisor) {
    unsigned int numCores = getNumCoresByHypervisorPath(pHypervisor->getFullPath());
    simtime_t **startTimesArray = getStartTimeByHypervisorPath(pHypervisor->getFullPath());
    simtime_t *timersArray = getTimerArrayByHypervisorPath(pHypervisor->getFullPath());
    bool* freeCoresArrayPtr = pHypervisor->getFreeCoresArrayPtr();

    for (int i=0; i<numCores; i++){
        if (freeCoresArrayPtr[i] && startTimesArray[i] != nullptr) {
            timersArray[i] += simTime() - *(startTimesArray[i]);
            delete startTimesArray[i];
            startTimesArray[i] = nullptr;
        } else if (!freeCoresArrayPtr[i] && startTimesArray[i] == nullptr){
            startTimesArray[i] = new SimTime(simTime());
        }
    }
}


void DataCenterManager::deallocateVmResources(std::string strVmId)
{
    Hypervisor *pHypervisor = getNodeHypervisorByVm(strVmId);

    if (pHypervisor==nullptr)
        error ("%s - Unable to deallocate VM. Wrong VM name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strVmId.c_str());

    pHypervisor->deallocateVmResources(strVmId);

    updateCpuUtilizationTimeForHypervisor(pHypervisor);
}

//TODO: asignar la vm que hace el timout al mensaje de la app. Duplicarlo y enviarlo.
void DataCenterManager::handleExecVmRentTimeout(cMessage *msg) {
    SM_UserAPP *pUserApp;
    Hypervisor *pHypervisor;

    std::string strUsername,
                strVmId,
                strAppName,
                strIp;

    bool bAlreadyFinished;

    SM_UserVM_Finish *pUserVmFinish;

    std::map<std::string, SM_UserAPP*>::iterator it;

    pUserVmFinish = dynamic_cast<SM_UserVM_Finish*>(msg);
    if (pUserVmFinish == nullptr)
        error ("%s - Unable to cast msg to SM_UserVM_Finish*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), msg->getName());

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - INIT" << endl;
    strVmId = pUserVmFinish->getStrVmId();

    strUsername = pUserVmFinish->getUserID();
    EV_INFO << "The rent of the VM [" << strVmId
                           << "] launched by the user " << strUsername
                           << " has finished" << endl;

    deallocateVmResources(strVmId);

    pUserApp = getUserAppRequestPerUser(strUsername);

    if (pUserApp == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app request message from the User!!").c_str());

//    acceptAppRequest(pUserApp, strVmId);

        //Check the Application status


        EV_INFO << "Last id gate: " << pUserApp->getLastGateId() << endl;
        EV_INFO
        << "Checking the status of the applications which are running over this VM"
        << endl;

        //Abort the running applications
        if (!pUserApp->allAppsFinished(strVmId))
          {
            EV_INFO << "Aborting running applications" << endl;
            abortAllApps(strVmId);
            pUserApp->abortAllApps(strVmId);
          }
        // Check the result and send it
        checkAllAppsFinished(pUserApp, strVmId);


        EV_INFO << "Freeing resources..." << endl;

}

void DataCenterManager::checkAllAppsFinished(SM_UserAPP* pUserApp, std::string strVmId) {
    std::string strUsername;

    if (pUserApp != nullptr)
      {
        strUsername = pUserApp->getUserID();
        if (pUserApp->allAppsFinished(strVmId))
          {
            if (pUserApp->allAppsFinishedOK(strVmId))
              {
                EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - All the apps corresponding with the user "
                << strUsername
                << " have finished successfully" << endl;

                pUserApp->printUserAPP();

                //Notify the user the end of the execution
                acceptAppRequest(pUserApp, strVmId);

              }
            else
              {
                EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                << " - All the apps corresponding with the user "
                << strUsername
                << " have finished with some errors" << endl;

                //Check the subscription queue
                //updateSubsQueue();

                //if (!pUserApp->getFinished())
                timeoutAppRequest(pUserApp, strVmId);  //Notify the user the end of the execution
              }

            //Delete the application on the hashmap
            //handlingAppsRqMap.erase(strUsername);
          }
        else
          {
            EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                    << " - Total apps finished: "
                    << pUserApp->getNFinishedApps() << " of "
                    << pUserApp->getAppArraySize() << endl;
          }
      }
    else
      {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
        << " - WARNING! Null pointer parameter "
        << strUsername << endl;
      }
}

void  DataCenterManager::acceptAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId)
{
    EV_INFO << "Sending vm end to the CP:" << userAPP_Rq->getUserID() << endl;

    SM_UserAPP* userAPP_Res = userAPP_Rq->dup();
    userAPP_Res->printUserAPP();

    userAPP_Res->setVmId(strVmId.c_str());
    userAPP_Res->setFinished(true);

    //Fill the message
    userAPP_Res->setIsResponse(true);
    userAPP_Res->setOperation(SM_APP_Rsp);
    userAPP_Res->setResult(SM_APP_Res_Accept);

    //Send the values
    sendResponseMessage(userAPP_Res);

}

void  DataCenterManager::timeoutAppRequest(SM_UserAPP* userAPP_Rq, std::string strVmId)
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

void DataCenterManager::clearVMReq (SM_UserVM*& userVM_Rq, int lastId)
{
    std::map<std::string, Hypervisor*>::iterator it;
    Hypervisor *pHypervisor;
    for(int i = 0; i < lastId ; i++)
      {
        VM_Request& vmRequest = userVM_Rq->getVms(i);
        cancelAndDelete(vmRequest.pMsg);
        vmRequest.pMsg = nullptr;

        deallocateVmResources(vmRequest.strVmId);

        //datacenterCollection->freeVmRequest(vmRequest.strVmId);
      }
}

SM_UserVM_Finish* DataCenterManager::scheduleRentingTimeout (std::string name, std::string strUserName, std::string strVmId, double rentTime)
{
    SM_UserVM_Finish *pMsg = new SM_UserVM_Finish();

    pMsg->setUserID(strUserName.c_str());
    pMsg->setName(name.c_str());

    if(!strVmId.empty())
        pMsg ->setStrVmId(strVmId.c_str());

    EV_INFO << "Scheduling Msg name " << pMsg << " at "<< simTime().dbl() + rentTime << endl;
    scheduleAt(simTime() + SimTime(rentTime), pMsg);

    return pMsg;
}

Hypervisor* DataCenterManager::selectNode (string strUserName, const VM_Request& vmRequest){
    VirtualMachine *pVMBase;
    Hypervisor *pHypervisor = nullptr;
    NodeResourceRequest *pResourceRequest;
    int numCoresRequested, numNodeTotalCores, numAvailableCores;
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;
    std::vector<Hypervisor*> vectorHypervisor;
    std::vector<Hypervisor*>::iterator itVector;
    bool bHandled;

    pVMBase = findVirtualMachine(vmRequest.strVmType);
    numCoresRequested = pVMBase->getNumCores();

    bHandled = false;
    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end() && !bHandled; ++itMap){
        numNodeTotalCores = itMap->first;
        if (numNodeTotalCores >= numCoresRequested) {
            vectorHypervisor = itMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && !bHandled; ++itVector) {
                pHypervisor = *itVector;
                numAvailableCores = pHypervisor->getAvailableCores();
                if (numAvailableCores >= numCoresRequested) {
                    pResourceRequest = generateNode(strUserName, vmRequest);
                    // TODO: Probablemente sea mejor mover esto al hypervisor. La asignación al map y que sea el hypervisor el que controle a que VM va.
                    // TODO: Finalmente debería devolver la IP del nodo y que el mensaje de la App llegue al nodo.
                    cModule *pVmAppVectorModule = pHypervisor->allocateNewResources(pResourceRequest);
                    if (pVmAppVectorModule!=nullptr) {
                        updateCpuUtilizationTimeForHypervisor(pHypervisor);
                        mapAppsVectorModulePerVm[vmRequest.strVmId] = pVmAppVectorModule;
                        int numMaxApps = pVmAppVectorModule->par("numApps");
                        bool *runningAppsArr = new bool[numMaxApps]{false};
                        mapAppsRunningInVectorModulePerVm[vmRequest.strVmId] = runningAppsArr;
                        bHandled = true;
                        return pHypervisor;
                    }

                }
            }

        }
    }

    return nullptr;
}



int DataCenterManager::calculateTotalCoresRequested(SM_UserVM* userVM_Rq)
{
    int nRet, nRequestedVms;
    VM_Request vmRequest;

    nRet=nRequestedVms=0;
    if(userVM_Rq != NULL)
    {
        nRequestedVms = userVM_Rq->getTotalVmsRequests();

        for(int i=0;i<nRequestedVms;i++)
        {
            vmRequest = userVM_Rq->getVms(i);

            nRet+=getTotalCoresByVmType(vmRequest.strVmType);
        }
    }
    EV_DEBUG << "User:" << userVM_Rq->getUserID() << " has requested: "<< nRet << " cores" << endl;

    return nRet;
}

int DataCenterManager::getTotalCoresByVmType(std::string strVmType)
{
    int nRet;
    VirtualMachine* pVmType;

    nRet=0;

    pVmType = findVirtualMachine(strVmType);

    if(pVmType != NULL)
    {
        nRet = pVmType->getNumCores();
    }

    return nRet;
}

NodeResourceRequest* DataCenterManager::generateNode(std::string strUserName, VM_Request vmRequest)
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

void  DataCenterManager::fillVmFeatures(std::string strVmType, NodeResourceRequest*& pNode)
{
    VirtualMachine* pVmType;

    pVmType = findVirtualMachine(strVmType);

    if(pVmType != NULL)
    {
        EV_INFO << "fillVmFeatures - Vm:" << strVmType << " cpus: "<< pVmType->getNumCores() << " mem: " << pVmType->getMemoryGb() <<endl;

        pNode->setTotalCpUs(pVmType->getNumCores());
        pNode->setTotalMemory(pVmType->getMemoryGb());
    }

}

void DataCenterManager::finish() {
    printFinal();
}

void DataCenterManager::printFinal(){
    std::map<std::string, std::tuple<unsigned int, simtime_t**, simtime_t*>>::iterator it;
    std::tuple<unsigned int, simtime_t**, simtime_t*> timersTuple;
    unsigned int numCores;
    simtime_t *timerArray = nullptr;
    simtime_t finalSimulationTime = simTime();
    const char* strName = getParentModule()->getName();
    int nTotalIndex = 0;

    for (it = mapCpuUtilizationTimePerHypervisor.begin(); it != mapCpuUtilizationTimePerHypervisor.end(); it++) {
        timersTuple = it->second;
        numCores = std::get<0>(timersTuple);
        timerArray = std::get<2>(timersTuple);

        for (int i=0; i<numCores; i++) {
            EV_FATAL << "#___cpuTime#" << strName << " " << nTotalIndex << " " << timerArray[i].dbl() << " " << timerArray[i].dbl() / finalSimulationTime.dbl() << endl;
            nTotalIndex++;
        }
    }
}

//SM_UserAPP_Finish* DataCenterManager::scheduleAppTimeout (std::string name, std::string strUserName, std::string strAppName, std::string strVmId, double totalTime)
//{
//    SM_UserAPP_Finish *pMsgFinish = new SM_UserAPP_Finish();
//
//    pMsgFinish->setUserID(strUserName.c_str());
//    pMsgFinish->setStrApp(strAppName.c_str());
//
//    if(!strVmId.empty())
//        pMsgFinish ->setStrVmId(strVmId.c_str());
//
//    pMsgFinish->setNTotalTime(totalTime);
//    pMsgFinish->setName(name.c_str());
//
//    EV_INFO << "Scheduling time rental Msg, " << strAppName << " at " << simTime().dbl() + totalTime << "s" << endl;
//    scheduleAt(simTime() + SimTime(totalTime), pMsgFinish);
//
//    return pMsgFinish;
//}

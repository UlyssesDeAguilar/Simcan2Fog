#include "../DataCentreManagerCost/DataCentreManagerCost.h"

#include "Management/utils/LogUtils.h"

Define_Module(DataCentreManagerCost);

void DataCentreManagerCost::initialize()
{
    checkReservedFirst = par("checkReservedFirst");

    // Init super-class
    DataCentreManagerBase::initialize();

    // FIXME: Should check this! parseConfig() does no longer exist
    parseDataCentreConfig();
}

void DataCentreManagerCost::initializeRequestHandlers()
{
    requestHandlers[SM_APP_Req_Resume] = [this](SIMCAN_Message *msg) -> void
    { return handleExtendVmAndResumeExecution(msg); };
    requestHandlers[SM_APP_Req_End] = [this](SIMCAN_Message *msg) -> void
    { return handleEndVmAndAbortExecution(msg); };
}

int DataCentreManagerCost::parseDataCentreConfig()
{
    int result;
    const char *dataCentresConfigChr;

    dataCentresConfigChr = par("dataCentreConfig");
    DataCentreReservationConfigParser dataCentreParser(dataCentresConfigChr);
    result = dataCentreParser.parse();
    if (result == SC_OK)
    {
        dataCentresBase = dataCentreParser.getResult();
    }
    return result;
}

int DataCentreManagerCost::initDataCentreMetadata()
{
    int result = SC_OK;
    DataCentreReservation *pDataCentreBase = dynamic_cast<DataCentreReservation *>(dataCentresBase[0]);
    cModule *pRackModule, *pBoardModule, *pNodeModule;
    int numBoards, numNodes, numCores, numTotalCores = 0;
    int numReserverNodes = pDataCentreBase->getReservedNodes();

    for (int nRackIndex = 0; nRackIndex < pDataCentreBase->getNumRacks(false); nRackIndex++)
    {
        Rack *pRackBase = pDataCentreBase->getRack(nRackIndex, false);
        // Generate rack name in the data centre
        string strRackName = "rackCmp_" + pRackBase->getRackInfo()->getName();
        pRackModule = getParentModule()->getSubmodule(strRackName.c_str(), nRackIndex);

        numBoards = pRackModule->par("numBoards");

        for (int nBoardIndex = 0; nBoardIndex < numBoards; nBoardIndex++)
        {
            pBoardModule = pRackModule->getSubmodule("board", nBoardIndex);
            numNodes = pBoardModule->par("numBlades");

            for (int nNodeIndex = 0; nNodeIndex < numNodes; nNodeIndex++)
            {
                pNodeModule = pBoardModule->getSubmodule("blade", nNodeIndex);

                if (numReserverNodes > 0)
                {
                    storeReservedNodeMetadata(pNodeModule);
                    numReserverNodes--;
                    numCores = 0;
                }
                else
                {
                    numCores = storeNodeMetadata(pNodeModule);
                }

                numTotalCores += numCores;
            }
        }
    }

    nTotalCores = nTotalAvailableCores = numTotalCores;

    return result;
}

int DataCentreManagerCost::storeReservedNodeMetadata(cModule *pNodeModule)
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

    // Store hypervisor pointers by number of cores
    mapHypervisorPerNodesReserved[numCores].push_back(pHypervisor);
    // Initialize cpu utilization timers
    mapCpuUtilizationTimePerHypervisor[pHypervisorModule->getFullPath()] = std::make_tuple(numCores, startTimeArray, timerArray);

    return numCores;
}

Hypervisor *DataCentreManagerCost::selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest)
{
    auto pCloudUser = dynamic_cast<const CloudUserPriority *>(findUserTypeById(userVM_Rq->getUserID()));
    Hypervisor *pHypervisor = nullptr;
    SM_UserVM_Cost *userVM_Rq_Cost = dynamic_cast<SM_UserVM_Cost *>(userVM_Rq);

    if (pCloudUser == nullptr)
        error("[%s] Wrong pCloudUser. Null pointer or wrong cloud user class!", LogUtils::prettyFunc(__FILE__, __func__).c_str());

    if (!checkReservedFirst)
    {
        pHypervisor = DataCentreManagerFirstFit::selectNode(userVM_Rq, vmRequest);
    }

    if (pHypervisor == nullptr && pCloudUser->getPriorityType() == Priority)
    {
        pHypervisor = selectNodeReserved(userVM_Rq_Cost, vmRequest);
    }

    if (pHypervisor == nullptr && checkReservedFirst)
    {
        pHypervisor = DataCentreManagerFirstFit::selectNode(userVM_Rq, vmRequest);
    }

    return pHypervisor;
}

Hypervisor *DataCentreManagerCost::selectNodeReserved(SM_UserVM_Cost *&userVM_Rq, const VM_Request &vmRequest)
{
    std::map<int, std::vector<Hypervisor *>>::iterator itMap;
    std::vector<Hypervisor *> vectorHypervisor;
    std::vector<Hypervisor *>::iterator itVector;
    bool bHandled;

    if (userVM_Rq == nullptr)
        return nullptr;

    std::string userId = userVM_Rq->getUserID();

    auto pVMBase = dataManager->searchVirtualMachine(vmRequest.strVmType);
    int numCoresRequested = pVMBase->getNumCores();

    bHandled = false;
    for (itMap = mapHypervisorPerNodesReserved.begin(); itMap != mapHypervisorPerNodesReserved.end() && !bHandled; ++itMap)
    {
        int numNodeTotalCores = itMap->first;
        if (numNodeTotalCores >= numCoresRequested)
        {
            vectorHypervisor = itMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && !bHandled; ++itVector)
            {
                Hypervisor *pHypervisor = *itVector;
                int numAvailableCores = pHypervisor->getAvailableCores();
                if (numAvailableCores >= numCoresRequested)
                {
                    NodeResourceRequest *pResourceRequest = generateNode(userId, vmRequest);

                    // TODO: Probablemente sea mejor mover esto al hypervisor. La asignaci�n al map y que sea el hypervisor el que controle a que VM va.
                    // TODO: Finalmente deber�a devolver la IP del nodo y que el mensaje de la App llegue al nodo.
                    cModule *pVmAppVectorModule = pHypervisor->allocateNewResources(pResourceRequest);
                    if (pVmAppVectorModule != nullptr)
                    {
                        updateCpuUtilizationTimeForHypervisor(pHypervisor);
                        mapAppsVectorModulePerVm[vmRequest.strVmId] = pVmAppVectorModule;
                        int numMaxApps = pVmAppVectorModule->par("numApps");
                        bool *runningAppsArr = new bool[numMaxApps]{false};
                        mapAppsRunningInVectorModulePerVm[vmRequest.strVmId] = runningAppsArr;
                        userVM_Rq->setBPriorized(true);
                        bHandled = true;
                        return pHypervisor;
                    }
                }
            }
        }
    }

    return nullptr;
}

void DataCentreManagerCost::handleExecVmRentTimeout(cMessage *msg)
{
    SM_UserAPP *pUserApp;
    Hypervisor *pHypervisor;

    std::string strUsername,
        strVmType,
        strVmId,
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

    strUsername = pUserVmFinish->getUserID();
    EV_INFO << "The rent of the VM [" << strVmId
            << "] launched by the user " << strUsername
            << " has finished" << '\n';

    // deallocateVmResources(strVmId);

    pUserApp = getUserAppRequestPerUser(strUsername);

    if (pUserApp == nullptr)
    {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] There is no app request message from the User!").c_str());
    }
    //    acceptAppRequest(pUserApp, strVmId);

    // Check the Application status

    EV_INFO << "Last id gate: " << pUserApp->getLastGateId() << '\n';
    EV_INFO
        << "Checking the status of the applications which are running over this VM"
        << '\n';

    // Abort the running applications
    if (!pUserApp->allAppsFinished(strVmId))
    {
        EV_INFO << "Aborting running applications" << '\n';
        abortAllApps(strVmId);
        pUserApp->abortAllApps(strVmId);
    }
    else
    {
        deallocateVmResources(strVmId);
        strVmType = pUserVmFinish->getStrVmType();
        nTotalAvailableCores += getTotalCoresByVmType(strVmType);
    }
    // Check the result and send it
    checkAllAppsFinished(pUserApp, strVmId);

    EV_INFO << "Freeing resources..." << '\n';
}

void DataCentreManagerCost::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    auto userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);
    if (userAPP_Rq == nullptr)
        error("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());

    std::string strVmId = userAPP_Rq->getVmId();
    if (strVmId.empty())
        return; // Original behavior, Should we warn the user ?

    std::string userId = userAPP_Rq->getUserID();
    SM_UserVM *userVmRequest = nullptr;

    // Try to find the user and his vm request
    try
    {
        userVmRequest = acceptedUsersRqMap.at(userId);
    }
    catch (std::out_of_range const &e)
    {
        EV_INFO << "WARNING! [" << LogUtils::prettyFunc(__FILE__, __func__) << "] The user: " << userId << "has not previously registered!!" << '\n';
        return;
    }

    for (int j = 0; j < userVmRequest->getVmsArraySize(); j++)
    {
        // Getting VM and scheduling renting timeout
        auto vmRequest = userVmRequest->getVms(j);
        // scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUsername, vmRequest.strVmId, vmRequest.nRentTime_t2);

        if (strVmId.compare(vmRequest.strVmId) == 0)
        {
            vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, userId, vmRequest, 3600);
            handleUserAppRequest(sm);
            return;
        }
    }
}

void DataCentreManagerCost::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    auto userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);
    if (userAPP_Rq == nullptr)
        error("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());

    std::string vmId = userAPP_Rq->getVmId();
    if (!vmId.empty())
    {
        EV_INFO << "Freeing resources..." << '\n';

        // Free the VM resources
        deallocateVmResources(vmId);

        // Delete ephemeral message
        delete sm;
    }
}

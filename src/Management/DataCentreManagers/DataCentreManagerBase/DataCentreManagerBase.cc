#include "DataCentreManagerBase.h"

#include "Management/utils/LogUtils.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace hypervisor;

// Define_Module(DataCentreManagerBase);

DataCentreManagerBase::~DataCentreManagerBase()
{
    acceptedVMsMap.clear();
    acceptedUsersRqMap.clear();
    handlingAppsRqMap.clear();
    mapAppsModulePerId.clear();
}

void DataCentreManagerBase::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
        // Init super-class
        CloudManagerBase::initialize();

        // Link app builder
        appBuilder = new DataCentreApplicationBuilder(); // FIXME: This will be deprecated soon
        appBuilder->setManager(this);

        // Get parameters from module
        showDataCentreConfig = par("showDataCentreConfig");
        nCpuStatusInterval = par("cpuStatusInterval");
        nActiveMachinesThreshold = par("activeMachinesThreshold");
        forecastActiveMachines = par("forecastActiveMachines");

        // Get gates
        networkGates.inBaseId = gate("networkIn")->getId();
        networkGates.outBaseId = gate("networkOut")->getId();

        localNetworkGates.inBaseId = gateHalf("localNetwork", cGate::INPUT)->getId();
        localNetworkGates.outBaseId = gateHalf("localNetwork", cGate::OUTPUT)->getId();
        
        // TODO: Study if these are really neccessary
        // Create and schedule auto events
        cpuManageMachinesMessage = new cMessage("MANAGE_MACHINES", MANAGE_MACHINES);
        cpuStatusMessage = new cMessage("CPU_STATUS", CPU_STATUS);
        scheduleAt(SimTime(), cpuManageMachinesMessage);
        scheduleAt(SimTime(), cpuStatusMessage);
        break;
    }
    case NEAR:
    {
        // Locate the ResourceManager
        resourceManager = check_and_cast<DcResourceManager *>(getModuleByPath("^.resourceManager"));
        resourceManager->setMinActiveMachines(par("minActiveMachines"));
        resourceManager->setReservedNodes(par("reservedNodes"));
        break;
    }
    case inet::InitStages::INITSTAGE_APPLICATION_LAYER:
    {
        // Set the global DC address into the resource manager
        resourceManager->setGlobalAddress(L3AddressResolver().resolve("^.networkAdapter"));
        break;
    }
    default:
        break;
    }
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

cGate *DataCentreManagerBase::getOutGate(cMessage *msg)
{
    int gateId = msg->getArrivalGate()->getId();

    // Check from which network the request came from
    if (gateId == networkGates.inBaseId)
        return gate(networkGates.outBaseId);
    else if (gateId == localNetworkGates.inBaseId)
        return gate(localNetworkGates.outBaseId);
    else
        error("Message received from an unknown gate [%s]", msg->getName());

    return nullptr;
}

void DataCentreManagerBase::initializeSelfHandlers()
{
    selfHandlers[CPU_STATUS] = [this](cMessage *msg) -> void
    { return handleCpuStatus(msg); };

    selfHandlers[MANAGE_MACHINES] = [this](cMessage *msg) -> void
    { return handleManageMachines(msg); };
}

void DataCentreManagerBase::initializeRequestHandlers()
{
    // TODO: Select the incoming traffic
    // Forward the request to the Hypervisor
    auto forwardRequest = [this](SIMCAN_Message *msg) -> void
    { sendRequestMessage(msg, localNetworkGates.outBaseId); };

    requestHandlers[SM_APP_Req] = forwardRequest;

    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRequestFits(msg); };

    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void
    { return handleVmSubscription(msg); };

    requestHandlers[SM_APP_Req_Resume] = [this](SIMCAN_Message *msg) -> void
    { return handleExtendVmAndResumeExecution(msg); };

    requestHandlers[SM_APP_Req_End] = [this](SIMCAN_Message *msg) -> void
    { return handleEndVmAndAbortExecution(msg); };
}

void DataCentreManagerBase::initializeResponseHandlers()
{
    // TODO: Select the outgoing traffic
    // Forward the responses from the Hypervisor
    auto forwardResponse = [this](SIMCAN_Message *msg) -> void
    { sendResponseMessage(msg); };

    // Intercept the vm timeout
    responseHandlers[SM_APP_Res_Timeout] = [this](SIMCAN_Message *msg) -> void
    { handleVmRentTimeout(msg); };
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

void DataCentreManagerBase::handleVmRequestFits(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << '\n';

    auto userVM_Rq = check_and_cast<SM_UserVM *>(sm);
    EV_INFO << userVM_Rq << '\n';

    // Check if is a VmRequest or a subscribe
    if (checkVmUserFit(userVM_Rq))
    {
        EV_INFO << "Accepting request from user:" << userVM_Rq->getUserId() << '\n';
        userVM_Rq->setResult(SM_VM_Res_Accept);
    }
    else
    {
        EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserId() << '\n';
        userVM_Rq->setResult(SM_VM_Res_Reject);
    }

    // Set response and operation type
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);

    // Send response
    sendResponseMessage(sm);
}

void DataCentreManagerBase::handleVmSubscription(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Subscribe operation" << '\n';

    auto userVM_Rq = check_and_cast<SM_UserVM *>(sm);

    if (checkVmUserFit(userVM_Rq))
    {
        // If it could fit, accept request
        EV_INFO << "Notifying request from user: " << userVM_Rq->getUserId() << '\n';
        EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';
        userVM_Rq->setResult(SM_APP_Sub_Accept);
    }
    else
    {
        // If it couldn't fit, reject request
        EV_INFO << "Notifying timeout from user:" << userVM_Rq->getUserId() << '\n';
        EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << '\n';
        userVM_Rq->setResult(SM_APP_Sub_Reject);
    }

    // Set response and operation type
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);

    // Send the response
    sendResponseMessage(userVM_Rq);
}

bool DataCentreManagerBase::checkVmUserFit(SM_UserVM *&userVM_Rq)
{
    assert_msg(userVM_Rq != nullptr, "checkVmUserFit nullptr guard failed\n");

    int nRequestedVms = userVM_Rq->getVmArraySize();
    std::string userId = userVM_Rq->getUserId();

    EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userId << '\n';

    // Before starting the process, it is neccesary to check if the
    int nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
    int nAvailableCores = getNTotalAvailableCores();

    if (nTotalRequestedCores > nAvailableCores)
    {
        EV_DEBUG << "checkVmUserFit - There isn't enough space: [" << userVM_Rq->getUserId() << nTotalRequestedCores
                 << " vs Available [" << nAvailableCores << "/" << getNTotalCores() << "]" << '\n';
        return false;
    }

    int nTotalCores = getNTotalCores();
    EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserId() << nTotalRequestedCores
             << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';

    // Start the deployment
    VmDeployment deployment(resourceManager, userVM_Rq);

    // Process all the vms
    for (int i = 0; i < nRequestedVms; i++)
    {
        uint32_t nodeIp;
        size_t bucketIndex;

        // Retrieve vm and select candidate
        auto vmRequest = userVM_Rq->getVm(i);
        auto vmSpecs = dataManager->searchVirtualMachine(vmRequest.vmType);

        // Select a node to deploy
        std::tie(nodeIp, bucketIndex) = selectNode(userVM_Rq, *vmSpecs);

        if (nodeIp != UINT32_MAX)
        {
            // Found, add to the deployment
            deployment.addNode(nodeIp, bucketIndex, &vmRequest, vmSpecs);
        }
        else
        {
            // Unable to allocate, rollback changes
            deployment.rollback();
            return false;
        }
    }

    // Store request as accepted -- In case of renting extenion this will be useful !
    acceptedUsersRqMap[userId] = userVM_Rq;
    deployment.commit();

    // The deployment was allocated successfully
    return true;
}

void DataCentreManagerBase::handleVmRentTimeout(SIMCAN_Message *sm)
{
    // TODO: Ledgure the suspension status
}

void DataCentreManagerBase::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    auto userAPP_Rq = check_and_cast<SM_UserAPP *>(sm);

    std::string strVmId = userAPP_Rq->getVmId();
    if (strVmId.empty())
        return; // Original behavior, Should we warn the user ?

    std::string userId = userAPP_Rq->getUserID();

    // FIXME: This mechanism is very unreliable and slow
    // The vm should be held in a suspended state until confirmation from user arrives
    // Only then the vm is reactivated or released completely
    // -> This also fixes the subscribe queue from the Cloud Provider!

    /*
    // Try to find the user and his vm request
    SM_UserVM *userVmRequest = acceptedUsersRqMap.at(userId);

    for (int j = 0; j < userVmRequest->getVmArraySize(); j++)
    {
        // Getting VM and scheduling renting timeout
        auto vmRequest = userVmRequest->getVm(j);
        // scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUsername, vmRequest.strVmId, vmRequest.nRentTime_t2);

        if (strVmId.compare(vmRequest.vmId) == 0)
        {
            vmRequest.pMsg = scheduleVmMsgTimeout(EXEC_VM_RENT_TIMEOUT, userId, vmRequest, 3600);
            handleUserAppRequest(sm);
            return;
        }
    }
    */
}

void DataCentreManagerBase::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    // This method is pretty much correct
    // Maybe we should correct the design (messages/protocol)
    // To just convey the necessary info: the vmId!

    auto userAPP_Rq = check_and_cast<SM_UserAPP *>(sm);
    std::string vmId = userAPP_Rq->getVmId();
    if (!vmId.empty())
    {
        EV_INFO << "Freeing resources..." << '\n';

        // Free the VM resources
        // deallocateVmResources(vmId);

        // Delete ephemeral message
        delete sm;
    }
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

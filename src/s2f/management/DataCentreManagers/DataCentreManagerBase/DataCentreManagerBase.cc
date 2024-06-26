#include "../../../management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"

#include "../../../management/utils/LogUtils.h"
#include "Management/utils/LogUtils.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace inet;
using namespace hypervisor;

Define_Module(DataCentreManagerBase);

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

        nodeSelectionStrategy = dc::SelectionStrategy::newStrategy(par("selectionStrategy"), this);
        if (nodeSelectionStrategy == nullptr)
            error("Unknown selection strategy");

        break;
    }
    case inet::InitStages::INITSTAGE_APPLICATION_LAYER:
    {
        // Set the global DC address into the resource manager
        resourceManager->setGlobalAddress(L3AddressResolver().addressOf(getModuleByPath("^.stack")));

        if (par("hasCloudEvents"))
        {
            // Prepare the event message template for cloud provider
            const char *topic = getParentModule()->getSubmodule("stack")->par("nodeTopic");
            eventTemplate.setChunkLength(inet::B(20));
            eventTemplate.setNodeTopic(topic);
            eventTemplate.setAvailableCores(resourceManager->getAvailableCores());

            // Allow for init
            Packet *packet = buildUpdateEvent();
            sendDelayed(packet, 1, gate("eventOut"));
        }
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

    // Print statistics
    printFinal();
}

Packet *DataCentreManagerBase::buildUpdateEvent()
{
    return new Packet("Update Event", makeShared<NodeEvent>(eventTemplate));
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
    auto forwardRequest = [this](SIMCAN_Message *msg) -> void
    { sendRequestMessage(msg, localNetworkGates.outBaseId); };

    requestHandlers[SM_APP_Req] = forwardRequest;

    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRequestFits(msg); };

    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void
    { return handleVmSubscription(msg); };

    // This one is special, it comes from the hypervisors
    requestHandlers[SM_ExtensionOffer] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRentTimeout(msg); };
}

void DataCentreManagerBase::initializeResponseHandlers()
{
    auto forwardResponse = [this](SIMCAN_Message *msg) -> void
    { sendResponseMessage(msg); };

    responseHandlers[SM_APP_Res_Accept] = forwardResponse;

    responseHandlers[SM_ExtensionOffer_Accept] = [this](SIMCAN_Message *msg) -> void
    { return handleExtendVmAndResumeExecution(msg); };

    responseHandlers[SM_ExtensionOffer_Reject] = [this](SIMCAN_Message *msg) -> void
    { return handleEndVmAndAbortExecution(msg); };
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

    SimTime currentTime = simTime();
    EV_FATAL << "#___machinesInUse#" << strName << " " << currentTime << " " << machinesInUse << '\n';
    EV_FATAL << "#___activeMachines#" << strName << " " << currentTime << " " << activeMachines << '\n';
    EV_FATAL << "#___activeOrInUseMachines#" << strName << " " << currentTime << " " << activeOrInUseMachines << '\n';

    // If not finished, reuse the same message MANAGE_MACHINES
    if (!bFinished)
        scheduleAt(currentTime + SimTime(nCpuStatusInterval, SIMTIME_S), msg);

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

    auto request = check_and_cast<SM_UserVM *>(sm);
    EV_INFO << request << '\n';

    // Check if is a VmRequest or a subscribe
    if (checkVmUserFit(request))
    {
        EV_INFO << "Accepting request from user:" << request->getUserId() << '\n';
        request->setResult(SM_VM_Res_Accept);
    }
    else
    {
        EV_INFO << "Reject VM request from user:" << request->getUserId() << '\n';
        request->setResult(SM_VM_Res_Reject);

        // Update the cloud provider to "uncommit" the resources
        eventTemplate.setAvailableCores(resourceManager->getAvailableCores());
        Packet *packet = buildUpdateEvent();
        send(packet, gate("eventOut"));
    }

    // Set response and operation type
    request->setIsResponse(true);
    request->setOperation(SM_VM_Req);

    // Send response
    request->setDestinationTopic(request->getReturnTopic());
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

        // Update the cloud provider to "uncommit" the resources
        eventTemplate.setAvailableCores(resourceManager->getAvailableCores());
        Packet *packet = buildUpdateEvent();
        send(packet, gate("eventOut"));
    }

    // Set response and operation type
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Sub);

    // Send the response
    userVM_Rq->setDestinationTopic(userVM_Rq->getReturnTopic());
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
    int nAvailableCores = resourceManager->getAvailableCores();

    if (nTotalRequestedCores > nAvailableCores)
    {
        EV_DEBUG << "checkVmUserFit - There isn't enough space: [" << userVM_Rq->getUserId() << nTotalRequestedCores
                 << " vs Available [" << nAvailableCores << "/" << resourceManager->getTotalCores() << "]" << '\n';
        return false;
    }

    int nTotalCores = resourceManager->getTotalCores();
    EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserId() << nTotalRequestedCores
             << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << '\n';

    // Start the deployment
    VmDeployment deployment(resourceManager, userVM_Rq);

    EV_DEBUG << "Attempting deployment" << "\n";
    // Process all the vms
    for (int i = 0; i < nRequestedVms; i++)
    {
        // Retrieve vm and select candidate
        VM_Request &vmRequest = userVM_Rq->getVmForUpdate(i);
        auto vmSpecs = dataManager->searchVirtualMachine(vmRequest.vmType);
        if (vmSpecs == nullptr)
            error("VmType %s not found in virtual machine definitions", vmRequest.vmType.c_str());

        // Select a node to deploy
        uint32_t nodeIp = nodeSelectionStrategy->selectNode(userVM_Rq, *vmSpecs);

        if (nodeIp != UINT32_MAX)
        {
            // Found, add to the deployment
            deployment.addNode(nodeIp, vmRequest, vmSpecs);
        }
        else
        {
            // Unable to allocate, rollback changes
            deployment.rollback();
            return false;
        }
    }
    EV_DEBUG << "Deployment was succesfull" << "\n";

    // Store request as accepted -- In case of renting extenion this will be useful !
    acceptedUsersRqMap[userId] = userVM_Rq->getReturnTopic();
    deployment.commit();

    // The deployment was allocated successfully
    return true;
}

void DataCentreManagerBase::handleVmRentTimeout(SIMCAN_Message *sm)
{
    // Cast and recover the context for the deployment
    auto extensionOffer = check_and_cast<SM_VmExtend *>(sm);
    // Send back to the corresponding user the extension request
    extensionOffer->setDestinationTopic(acceptedUsersRqMap.at(extensionOffer->getUserId()).c_str());
    sendRequestMessage(extensionOffer, gate("networkOut"));
}

void DataCentreManagerBase::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    auto response = check_and_cast<SM_VmExtend *>(sm);
    EV << "User: " << response->getUserId() << " accepted the extension of vm: "
       << response->getVmId() << " for " << response->getExtensionTime() << "\n";

    auto routingInfo = check_and_cast<RoutingInfo *>(response->getControlInfo());
    resourceManager->resumeVm(routingInfo->getDestinationUrl().getLocalAddress().getInt(), response->getVmId(), response->getExtensionTime());
    delete response;
}

void DataCentreManagerBase::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    auto response = check_and_cast<SM_VmExtend *>(sm);
    EV << "User: " << response->getUserId() << " decided to not extend vm " << response->getVmId() << ". Freeing resources\n";

    auto routingInfo = check_and_cast<RoutingInfo *>(response->getControlInfo());
    resourceManager->deallocateVm(routingInfo->getDestinationUrl().getLocalAddress().getInt(), response->getVmId());
    delete response;

    // Notifiy the Cloud provider that the resources were finally freed
    eventTemplate.setAvailableCores(resourceManager->getAvailableCores());
    Packet *packet = buildUpdateEvent();
    send(packet, gate("eventOut"));
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

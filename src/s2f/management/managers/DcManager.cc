#include "DcManager.h"

#include "s2f/management/utils/LogUtils.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace inet;
using namespace hypervisor;

Define_Module(DcManager);

void DcManager::initialize(int stage)
{
    if (stage == LOCAL)
    {
        // Init super-class
        ManagerBase::initialize();

        // Get parameters from module
        showDataCentreConfig = par("showDataCentreConfig");
        nCpuStatusInterval = par("cpuStatusInterval");
        nActiveMachinesThreshold = par("activeMachinesThreshold");
        forecastActiveMachines = par("forecastActiveMachines");
        zone = par("zone");
        nodeName = par("nodeName");
        
        // Get gates
        networkGates.inBaseId = gate("networkIn")->getId();
        networkGates.outBaseId = gate("networkOut")->getId();
        queueGates.inBaseId = gate("queueIn")->getId();
        queueGates.outBaseId = gate("queueOut")->getId();

        // TODO: Study if these are really neccessary
        // Create and schedule auto events
        cpuManageMachinesMessage = new cMessage("MANAGE_MACHINES", MANAGE_MACHINES);
        cpuStatusMessage = new cMessage("CPU_STATUS", CPU_STATUS);
        scheduleAt(SimTime(), cpuManageMachinesMessage);
        scheduleAt(SimTime(), cpuStatusMessage);
    }
    else if (stage == NEAR)
    {
        // Locate the ResourceManager
        resourceManager = check_and_cast<ResourceManager *>(getModuleByPath(par("resourceManagerPath")));
        netDb = check_and_cast<SimpleNetDb *>(getModuleByPath(par("netDbPath")));
        resourceManager->setMinActiveMachines(par("minActiveMachines"));
        netDb->registerHost("manager", getModuleByPath(par("netCardPath"))->par("L2Address"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        // Set the global DC address into the resource manager
        if (par("hasCloudEvents"))
        {
            // Prepare the event message template and send to the cloud provider
            const char *topic = getParentModule()->getSubmodule("stack")->par("nodeTopic");
            eventTemplate.setDestinationTopic("cloudProvider");
            eventTemplate.setReturnTopic(topic);
            eventTemplate.setZone(zone);
            sendUpdateToCloudProvider();
        }
    }
}

void DcManager::finish()
{
    // Delete the events
    cancelAndDelete(cpuManageMachinesMessage);
    cancelAndDelete(cpuStatusMessage);
    acceptedVMsMap.clear();
    acceptedUsersRqMap.clear();
}

void DcManager::handleMessage(cMessage *msg)
{
    ManagerBase::handleMessage(L2Protocol::tryDecapsulate(msg));
}

cGate *DcManager::getOutGate(cMessage *msg)
{
    int gateId = msg->getArrivalGate()->getId();

    // Check from which network the request came from
    if (gateId == networkGates.inBaseId)
        return gate(networkGates.outBaseId);
    else if (gateId == queueGates.inBaseId)
        return gate(queueGates.outBaseId);
    else
        error("Message received from an unknown gate [%s]", msg->getName());

    return nullptr;
}

void DcManager::initializeSelfHandlers()
{
    selfHandlers[CPU_STATUS] = [this](cMessage *msg) -> void
    { return handleCpuStatus(msg); };

    selfHandlers[MANAGE_MACHINES] = [this](cMessage *msg) -> void
    { return handleManageMachines(msg); };
}

void DcManager::initializeRequestHandlers()
{
    requestHandlers[SM_APP_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleApplicationRequest(msg); };

    requestHandlers[SM_VM_Req] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRequestFits(msg); };

    requestHandlers[SM_VM_Sub] = [this](SIMCAN_Message *msg) -> void
    { return handleVmSubscription(msg); };

    // This one is special, it comes from the hypervisors
    requestHandlers[SM_ExtensionOffer] = [this](SIMCAN_Message *msg) -> void
    { return handleVmRentTimeout(msg); };
}

void DcManager::initializeResponseHandlers()
{
    auto forwardResponse = [this](SIMCAN_Message *msg) -> void
    { sendResponseMessage(msg); };

    responseHandlers[SM_APP_Res_Accept] = forwardResponse;

    responseHandlers[SM_ExtensionOffer_Accept] = [this](SIMCAN_Message *msg) -> void
    { return handleExtendVmAndResumeExecution(msg); };

    responseHandlers[SM_ExtensionOffer_Reject] = [this](SIMCAN_Message *msg) -> void
    { return handleEndVmAndAbortExecution(msg); };
}

void DcManager::handleCpuStatus(cMessage *msg)
{
    const char *strName = getParentModule()->getName();
    EV_FATAL << "#___cpuUsage#" << strName << " " << simTime().dbl() << " " << resourceManager->getAggregateCpuUsage() << "\n";

    // If not finished, reuse the same message CPU_STATUS
    if (!bFinished)
    {
        scheduleAt(simTime() + SimTime(nCpuStatusInterval, SIMTIME_S), msg);
    }
}

void DcManager::handleManageMachines(cMessage *msg)
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

void DcManager::manageActiveMachines()
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

void DcManager::handleVmRequestFits(SIMCAN_Message *sm)
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
        sendUpdateToCloudProvider();
    }

    // Set response and operation type
    request->setIsResponse(true);
    request->setOperation(SM_VM_Req);

    // Send response
    request->setDestinationTopic(request->getReturnTopic());
    sendResponseMessage(sm);
}

void DcManager::handleVmSubscription(SIMCAN_Message *sm)
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
        sendUpdateToCloudProvider();
    }

    // Set response and operation type
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Sub);

    // Send the response
    userVM_Rq->setDestinationTopic(userVM_Rq->getReturnTopic());
    sendResponseMessage(userVM_Rq);
}

bool DcManager::checkVmUserFit(SM_UserVM *&userVM_Rq)
{
    ASSERT2(userVM_Rq != nullptr, "checkVmUserFit nullptr guard failed\n");
    const std::vector<ResourceManager::NodeVmRecord> *allocRecords = resourceManager->allocateVms(userVM_Rq);

    if (!allocRecords)
        return false;
    
    acceptedUsersRqMap[userVM_Rq->getUserId()] = userVM_Rq->getReturnTopic();

    if (allocRecords->size() == 1)
        dispatchVm(allocRecords->at(0), userVM_Rq);
    else
    {
        dispatchVms(allocRecords, userVM_Rq);
    }

    delete allocRecords;
    return true;
}

void DcManager::dispatchVm(const ResourceManager::NodeVmRecord &record, SM_UserVM *userVM_Rq)
{
    auto &node = resourceManager->getNode(record.nodeIndex);
    auto &response = userVM_Rq->getVmForUpdate(0).response;
    response.state = VM_Response::ACCEPTED;
    response.startTime = simTime().dbl();
    response.ip = generateHypervisorUrl(netDb->getHostName(node.address)).c_str();
    sendToLocalNetwork(userVM_Rq->dup(), record.nodeIndex);
}

void DcManager::dispatchVms(const std::vector<ResourceManager::NodeVmRecord> *allocRecords, SM_UserVM *userVM_Rq)
{
    std::map<int, SM_UserVM *> hyperVmMap;
    int i = 0;

    for (auto &record : *allocRecords)
    {
        auto &node = resourceManager->getNode(record.nodeIndex);
        auto &response = userVM_Rq->getVmForUpdate(i).response;
        response.state = VM_Response::ACCEPTED;
        response.startTime = simTime().dbl();
        response.ip = generateHypervisorUrl(netDb->getHostName(node.address)).c_str();

        auto iter = hyperVmMap.find(record.nodeIndex);
        if (iter == hyperVmMap.end())
        {
            auto pair = hyperVmMap.insert(std::pair<int, SM_UserVM *>(record.nodeIndex, new SM_UserVM()));
            pair.first->second->appendVm(userVM_Rq->getVm(i));
        }
        else
            iter->second->appendVm(userVM_Rq->getVm(i));
        i++;
    }

    for (auto hyper : hyperVmMap)
        sendToLocalNetwork(hyper.second, hyper.first);
}

void DcManager::handleApplicationRequest(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Application Request" << '\n';
    auto appRequest = check_and_cast<SM_UserAPP *>(sm);
    int address = resolveHypervisorUrl(appRequest->getHypervisorUrl());
    sendToLocalNetwork(appRequest, address);
}

void DcManager::handleVmRentTimeout(SIMCAN_Message *sm)
{
    // Cast and recover the context for the deployment
    auto extensionOffer = check_and_cast<SM_VmExtend *>(sm);
    // Send back to the corresponding user the extension request
    extensionOffer->setDestinationTopic(acceptedUsersRqMap.at(extensionOffer->getUserId()).c_str());
    sendRequestMessage(extensionOffer, queueGates.outBaseId);
}

void DcManager::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    auto response = check_and_cast<SM_VmExtend *>(sm);
    EV << "User: " << response->getUserId() << " accepted the extension of vm: "
       << response->getVmId() << " for " << response->getExtensionTime() << "\n";

    int address = resolveHypervisorUrl(response->getHypervisorUrl());
    sendToLocalNetwork(response, address);
}

void DcManager::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    auto response = check_and_cast<SM_VmExtend *>(sm);
    EV << "User: " << response->getUserId() << " decided to not extend vm " << response->getVmId() << ". Freeing resources\n";

    int address = resolveHypervisorUrl(response->getHypervisorUrl());
    sendToLocalNetwork(response, address);

    // Notifiy the Cloud provider that the resources were finally freed
    sendUpdateToCloudProvider();
}

void DcManager::sendUpdateToCloudProvider()
{
    eventTemplate.setAvailableCores(resourceManager->getAvailableCores());
    eventTemplate.setAvailableRam(resourceManager->getAvailableRamMiB());
    eventTemplate.setAvailableDisk(resourceManager->getAvailableDiskMiB());
    send(eventTemplate.dup(), queueGates.outBaseId);
}

void DcManager::sendToLocalNetwork(cMessage *msg, int destinationAddress)
{
    send(L2Protocol::encapsulate(msg, destinationAddress), networkGates.outBaseId);
}

opp_string DcManager::generateHypervisorUrl(const char *hostName)
{
    return opp_string(nodeName) + opp_string("::") + hostName;
}

int DcManager::resolveHypervisorUrl(const char *hypervisorUrl)
{
    if (!hypervisorUrl)
        error("Invalid hypervisor url: %s", hypervisorUrl);
    
    cStringTokenizer tokenizer(hypervisorUrl, "::");
    if (!tokenizer.hasMoreTokens())
        error("Invalid hypervisor url: %s", hypervisorUrl);

    const char *nodeName = tokenizer.nextToken();

    if (!tokenizer.hasMoreTokens())
        error("Invalid hypervisor url: %s", hypervisorUrl);

    const char *hostName = tokenizer.nextToken();

    if (strcmp(nodeName, nodeName) != 0)
        error("Wrong node (%s) for this hypervisor URL: %s", nodeName, hypervisorUrl);

    return netDb->getAddress(hostName);
}

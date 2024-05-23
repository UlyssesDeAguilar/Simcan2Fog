#include "BaseUserModel.h"
#include "Management/UserGenerators/UserGenerator_simple/UserGenerator_simple.h"

void BaseUserModel::handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance)
{
    // NOTE: We're assuming there is only ACCEPT/REJECT

    if (vmRequest->getResult() == SM_VM_Res_Accept)
    {
        // Update the status
        updateVmsStatus(userInstance, vmRequest->getVmId(), vmAccepted);

        // Emit statistics
        driver.emit(driver.responseSignal, userInstance.getNId());
        driver.emit(driver.executeSignal[""], userInstance.getNId());

        userInstance.getInstanceTimesForUpdate().initExec = simTime();

        // Set to false - did not had to subscribe
        userInstance.setSubscribe(false);

        // submitService(vmRequest);
        deployApps(vmRequest, userInstance);
    }
    else
    {
        // Update the status
        updateVmsStatus(userInstance, vmRequest->getVmId(), vmIdle);

        // Update statistics
        driver.emit(driver.responseSignal, userInstance.getNId());
        driver.emit(driver.subscribeSignal[""], userInstance.getNId());

        // Register times
        userInstance.getInstanceTimesForUpdate().initExec = simTime();
        userInstance.startSubscription();

        // Register that the user had to subscribe
        userInstance.setSubscribe(true);

        // Subscribe to cloud provider
        vmRequest->setIsResponse(false);
        vmRequest->setOperation(SM_VM_Sub);
        vmRequest->setDestinationTopic("CloudProvider");
        driver.sendRequestMessage(vmRequest, driver.toCloudProviderGate);
    }
}

void BaseUserModel::handleResponseSubscription(SM_UserVM *vmRequest, CloudUserInstance &userInstance)
{
    // NOTE: We're assuming there is only ACCEPT/REJECT (TIMEOUT)

    if (vmRequest->getResult() == SM_APP_Sub_Accept)
    {
        // Update the status
        const std::string &vmId = vmRequest->getVmId();

        updateVmsStatus(userInstance, vmId, vmAccepted);

        if (!vmId.empty())
            driver.extensionTimeHashMap.at(vmId)++;

        // Subscription time monitoring
        userInstance.endSubscription();

        driver.emit(driver.notifySignal[vmRequest->getVmId()], userInstance.getNId());
        driver.emit(driver.executeSignal[vmRequest->getVmId()], userInstance.getNId());

        if (strcmp(vmRequest->getVmId(), "") == 0) // If well done, should not be possible!
            userInstance.getInstanceTimesForUpdate().initExec = simTime();

        // Deploy the apps
        deployApps(vmRequest, userInstance);
    }
    else
    {
        // Update the status
        updateVmsStatus(userInstance, vmRequest->getVmId(), vmFinished);

        userInstance.endSubscription();
        userInstance.setTimeoutMaxSubscribed();

        // Emit statistics
        driver.emit(driver.timeoutSignal[vmRequest->getVmId()], userInstance.getNId());
    }

    // deleteIfEphemeralMessage(vmRequest); WEIRDCHECK1: If well done, should not be possible!
}

void BaseUserModel::handleResponseAppRequest(SM_UserAPP *appRequest, CloudUserInstance &userInstance)
{
    // NOTE: We assume ACCEPT/TIMEOUT
    std::string vmId(appRequest->getVmId());

    if (appRequest->getResult() == SM_APP_Res_Accept)
    {
        updateVmsStatus(userInstance, vmId, vmFinished);
        driver.emit(driver.okSignal[vmId], userInstance.getNId());
    }
    else
        driver.error("Unexpected return result on Applications: UserBaseModel");
}

void BaseUserModel::handleVmExtendRequest(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance)
{
    // Individual VM timeout -> Rescue or not rescue?
    driver.emit(driver.failSignal[extensionOffer->getVmId()], userInstance.getNId());

    auto response = extensionOffer->dup();
    response->setIsResponse(false);
    response->setResult(0);

    if (!decidesToRescueVm(extensionOffer, userInstance))
    {
        // Update the vms status
        updateVmsStatus(userInstance, extensionOffer->getVmId(), vmFinished);
        response->setResult(SM_ExtensionOffer_Reject);
        response->setAccepted(false);
    }
    else
    {
        // Decided to recover the vm and suscribe
        driver.extensionTimeHashMap.at(extensionOffer->getVmId())++;
        response->setResult(SM_ExtensionOffer_Accept);
        response->setAccepted(true);
        response->setExtensionTime(3600); // FIXME: Original Simcan2Cloud Behavior -- It's parameterizable
    }

    // Send the response to the endpoint
    response->setIsResponse(true);
    response->setDestinationTopic(extensionOffer->getReturnTopic());
    driver.sendRequestMessage(response, driver.toCloudProviderGate);

    delete extensionOffer;
}

void BaseUserModel::deployApps(SM_UserVM *vmRequest, CloudUserInstance &userInstance)
{
    UserAppBuilder builder;

    /*
      Bear in mind that the CloudUserInstance checks that for each app collection there's an vmInstance
      It's entirely possible that there aren't enough app collections, so they are the limiting factor here
     */
    for (int i = 0; i < userInstance.getNumberAppCollections(); i++)
    {
        AppInstanceCollection *appCollection = userInstance.getAppCollection(i);
        const VM_Request &vmRq = vmRequest->getVm(i);

        VM_Response *vmAllocation;
        if (!vmRequest->getResponse(i, &vmAllocation))
            driver.error("Error in model: The transactional deployment did not work?");

        int appsForVm = userInstance.getAppCollectionSize(i);

        // FIXME: This is a bit of a magic number, but it has to do with Simcan2Cloud design limitations
        if (appsForVm > 5)
            driver.error("EINVAL");

        // Send each app to each VM
        for (int j = 0; j < appsForVm; j++)
        {
            auto pAppInstance = appCollection->getInstance(j);
            const std::string &type = pAppInstance->getAppName();
            const std::string & instanceId = pAppInstance->getAppInstanceId();

            // auto nPrice = pRes->price;

            // Check if T2 <T3
            if (driver.bMaxStartTime_t1_active && userInstance.getRentTimes().maxStartTime < vmAllocation->startTime)
            {
                EV_INFO << "The maximum start rent time provided by the cloudprovider is greater than the maximum required by the user: "
                        << userInstance.getRentTimes().maxStartTime << " < " << vmAllocation->startTime << '\n';
            }
            else
            {
                builder.createNewAppRequest(instanceId + vmRq.vmId, type, vmAllocation->ip, vmRq.vmId, vmAllocation->startTime);
            }
        }
    }

    // Finish building the deployment
    std::vector<SM_UserAPP *> *appRequests = builder.finish(vmRequest->getUserId(), vmRequest->getReturnTopic());

    // For each different "ServiceURL" we deploy the vms we requested
    for (auto request : *appRequests)
        driver.sendRequestMessage(request, driver.toCloudProviderGate);

    delete appRequests;
}

void BaseUserModel::updateVmsStatus(CloudUserInstance &userInstance, const std::string &vmId, tVmState stateNew)
{
    // Iterate though collections
    for (int i = 0; i < userInstance.getNumberVmCollections(); i++)
    {
        for (const auto &instance : userInstance.getVmCollection(i)->allInstances())
        {
            if (vmId.empty() || vmId == instance->getVmInstanceId()) // WEIRDCHECK1 -- To be fixed by rescue operation!
            {
                tVmState stateOld = instance->getState();

                if (stateOld != vmFinished && stateNew == vmFinished)
                    userInstance.addFinishedVMs(1);
                else if (stateOld == vmFinished && stateNew != vmFinished)
                    userInstance.addFinishedVMs(-1);

                instance->setState(stateNew);
            }
        }
    }
}

bool BaseUserModel::decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance)
{
    return ((double)rand() / (RAND_MAX)) <= driver.offerAcceptanceRate;
}

void BaseUserModel::deleteIfEphemeralMessage(SIMCAN_Message *msg)
{
    SM_UserVM *userVm = dynamic_cast<SM_UserVM *>(msg);
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    string strVmId;

    if (userVm != nullptr)
        strVmId = userVm->getVmId();

    if (userApp != nullptr)
        strVmId = userApp->getVmId();

    if (!strVmId.empty())
        delete msg;
}
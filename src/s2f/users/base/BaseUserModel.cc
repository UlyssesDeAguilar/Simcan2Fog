#include "BaseUserModel.h"

#include "s2f/users/generators/UserGenerator_simple.h"

void BaseUserModel::handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance)
{
    // NOTE: We're assuming there is only ACCEPT/REJECT

    if (vmRequest->getResult() == SM_VM_Res_Accept)
    {
        // Update the status
        userInstance.updateVmInstanceStatus(vmRequest, vmAccepted);

        // Emit statistics
        driver.emit(driver.responseSignal, userInstance.getNId());
        driver.emit(driver.executeSignal[""], userInstance.getNId());

        userInstance.startExecution();

        // submitService(vmRequest);
        deployApps(vmRequest, userInstance);
    }
    else
    {
        // Update the status
        userInstance.updateVmInstanceStatus(vmRequest, vmIdle);

        // Update statistics
        driver.emit(driver.responseSignal, userInstance.getNId());
        driver.emit(driver.subscribeSignal[""], userInstance.getNId());

        // Start the subscription
        userInstance.startSubscription();

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

        // Update the status
        userInstance.updateVmInstanceStatus(vmRequest, vmAccepted);

        if (!vmId.empty())
            driver.extensionTimeHashMap.at(vmId)++;

        // Subscription time monitoring
        userInstance.endSubscription();

        driver.emit(driver.notifySignal[vmRequest->getVmId()], userInstance.getNId());
        driver.emit(driver.executeSignal[vmRequest->getVmId()], userInstance.getNId());

        userInstance.startExecution();

        // Deploy the apps
        deployApps(vmRequest, userInstance);
    }
    else
    {
        // Update the status
        userInstance.updateVmInstanceStatus(vmRequest, vmFinished);

        userInstance.endSubscription();
        userInstance.setTimeoutMaxSubscribed();

        // Emit statistics
        driver.emit(driver.timeoutSignal[vmRequest->getVmId()], userInstance.getNId());
    }
}

void BaseUserModel::handleResponseAppRequest(SM_UserAPP *appRequest, CloudUserInstance &userInstance)
{
    // NOTE: We assume ACCEPT/TIMEOUT

    if (appRequest->getResult() == SM_APP_Res_Accept)
    {
        userInstance.updateVmInstanceStatus(appRequest->getVmId(), vmFinished);
        driver.emit(driver.okSignal[std::string(appRequest->getVmId())], userInstance.getNId());
    }
    else
        driver.error("Unexpected return result on Applications: UserBaseModel");

    delete appRequest;
}

void BaseUserModel::handleVmExtendRequest(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance)
{
    // Individual VM timeout -> Rescue or not rescue?
    driver.emit(driver.failSignal[extensionOffer->getVmId()], userInstance.getNId());

    SM_VmExtend *response = extensionOffer->dup();

    bool finished = (userInstance.isFinished() || userInstance.getVmInstanceState(extensionOffer->getVmId()) == vmFinished);
    if (finished || !decidesToRescueVm(extensionOffer, userInstance))
    {
        EV_INFO << "User " << userInstance.getNId() << ": closing vm: " << extensionOffer->getVmId();
        // Update the status
        userInstance.updateVmInstanceStatus(extensionOffer->getVmId(), vmFinished);
        response->setResult(SM_ExtensionOffer_Reject);
        response->setAccepted(false);
    }
    else
    {
        EV_INFO << "User " << userInstance.getNId() << ": rescuing vm: " << extensionOffer->getVmId();
        // Decided to recover the vm and suscribe
        driver.extensionTimeHashMap.at(extensionOffer->getVmId())++;
        response->setResult(SM_ExtensionOffer_Accept);
        response->setAccepted(true);
        response->setExtensionTime(3600); // FIXME: Original Simcan2Cloud Behavior -- It's parameterizable
    }

    // Note that control info objects are not cloned across cMessage's
    auto routingInfo = check_and_cast<RoutingInfo *>(extensionOffer->getControlInfo());
    auto newRoutingInfo = new RoutingInfo();
    newRoutingInfo->setDestinationUrl(routingInfo->getSourceUrl());
    newRoutingInfo->setSourceUrl(ServiceURL(0));

    // Send the response to the endpoint
    response->setControlInfo(newRoutingInfo);
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
            const std::string &instanceId = pAppInstance->getAppInstanceId();

            // auto nPrice = pRes->price;

            // Check if T2 <T3
            if (driver.bMaxStartTime_t1_active && userInstance.getRentTimes().maxStartTime < vmAllocation->startTime)
            {
                EV_INFO << "The maximum start rent time provided by the cloudprovider is greater than the maximum required by the user: "
                        << userInstance.getRentTimes().maxStartTime << " < " << vmAllocation->startTime << '\n';
            }
            else
            {
                builder.createNewAppRequest((instanceId + vmRq.vmId).c_str(), type.c_str(), vmRq.vmId.c_str(), vmAllocation->startTime);
            }
        }
    }

    // For each vm which is unmapped, mark as already finished
    for (int i = 0; i < vmRequest->getVmArraySize(); i++)
    {
        const VM_Request &vmRq = vmRequest->getVm(i);
        userInstance.updateVmInstanceStatus(vmRq.vmId.c_str(), vmFinished);
    }

    // Finish building the deployment
    std::vector<SM_UserAPP *> *appRequests = builder.finish(vmRequest->getUserId());

    // For each different "ServiceURL" we deploy the vms we requested
    for (auto request : *appRequests){
    	request->setDestinationTopic(vmRequest->getReturnTopic());
        driver.sendRequestMessage(request, driver.toCloudProviderGate);
    }
    delete appRequests;
}

bool BaseUserModel::decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance)
{
    return ((double)rand() / (RAND_MAX)) <= driver.offerAcceptanceRate;
}

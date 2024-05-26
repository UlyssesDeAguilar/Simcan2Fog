#include "CloudUserModel.h"
#include "Management/UserGenerators/UserGeneratorCost/UserGeneratorCost.h"

void CloudUserModel::handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance)
{
    auto pCloudUser = driver.getDataManager()->searchUser(userInstance.getType());
    auto vmRequestCost = reinterpret_cast<SM_UserVM_Cost *>(vmRequest);
    auto driverCost = reinterpret_cast<UserGeneratorCost *>(&driver);

    if (vmRequest->getResult() == SM_VM_Res_Accept)
    {

        // Check the response and proceed with the next action
        if (pCloudUser->getPriorityType() == Priority && vmRequestCost->getBPriorized())
            driverCost->priorizedHashMap[vmRequest->getUserId()] = true;

        BaseUserModel::handleResponseVmRequest(vmRequest, userInstance);
    }
    else
    {
        // Check the response and proceed with the next action
        if (pCloudUser->getPriorityType() == Priority)
        {
            // Update the status -- In contrast to the BaseUser this ends the "cycle of requests"
            userInstance.updateVmInstanceStatus(vmRequest->getVmId(), vmFinished);

            // Update priorized
            if (vmRequestCost->getBPriorized())
                driverCost->priorizedHashMap[vmRequest->getUserId()] = true;

            driver.emit(driver.responseSignal, userInstance.getNId());
            userInstance.startExecution();
            userInstance.setTimeoutMaxSubscribed();
        }
        else
        {
            BaseUserModel::handleResponseVmRequest(vmRequest, userInstance);
        }
    }
}

bool CloudUserModel::decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance)
{
    auto driverCost = reinterpret_cast<UserGeneratorCost *>(&driver);
    auto userPriorityInstance = reinterpret_cast<CloudUserInstancePriority &>(userInstance);

    if (userPriorityInstance.isBCredit())
        userPriorityInstance.setBCredit(driverCost->offerAcceptanceDistribution->doubleValue() <= 0.9);

    return userPriorityInstance.isBCredit();
}

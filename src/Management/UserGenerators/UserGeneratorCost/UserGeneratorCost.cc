//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "UserGeneratorCost.h"
#include "Management/utils/LogUtils.h"

Define_Module(UserGeneratorCost);

void UserGeneratorCost::initialize()
{
    UserGenerator_simple::initialize();

    offerAcceptanceDistribution = &par("offerAcceptanceDistribution");
    offerCostIncrease = par("offerCostIncrease");

    initializeHashMaps();
}

void UserGeneratorCost::initializeHashMaps()
{
    // Accross all users
    for (const auto &userInstance : userInstances)
    {
        auto userInstanceId = userInstance->getId();
        priorizedHashMap[userInstanceId] = false;

        // Across all collections and their instances
        for (const auto &vmCollection : userInstance->allVmCollections())
        {
            for (const auto &vm : vmCollection->allInstances())
            {
                auto vmId = vm->getVmInstanceId();
                extensionTimeHashMap[vmId] = 0;
            }
        }
    }
}

CloudUserInstance *UserGeneratorCost::handleResponseVmAccept(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserVM_Cost *userVm = dynamic_cast<SM_UserVM_Cost *>(userVm_RAW);

    if (userVm == nullptr)
        error("Could not cast SIMCAN_Message to SM_UserVM_Cost (wrong operation code or message class?)");

    pUserInstance = userHashMap.at(userVm->getUserId());

    if (pUserInstance == nullptr)
        error("User instance not found!");

    auto pCloudUser = dataManager->searchUser(pUserInstance->getType());

    if (pCloudUser == nullptr)
        error("Could not cast CloudUser* to CloudUserPriority* (wrong user id, user type or user class?)");

    // Check the response and proceed with the next action
    if (pCloudUser->getPriorityType() == Priority && userVm->getBPriorized())
        priorizedHashMap[userVm->getUserId()] = true;

    UserGenerator_simple::handleResponseVmAccept(userVm_RAW);

    return pUserInstance;
}

CloudUserInstance *UserGeneratorCost::handleResponseVmReject(SIMCAN_Message *userVm_RAW)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserVM_Cost *userVm = dynamic_cast<SM_UserVM_Cost *>(userVm_RAW);

    if (userVm != nullptr)
    {

        pUserInstance = userHashMap.at(userVm->getUserId());

        if (pUserInstance == nullptr)
            error("User instance not found!");

        auto pCloudUser = dynamic_cast<const CloudUserPriority *>(dataManager->searchUser(pUserInstance->getType()));

        // Check the response and proceed with the next action
        if (pCloudUser != nullptr && pCloudUser->getPriorityType() == Priority)
        {
            EV_INFO << __func__ << " - Response message" << endl;

            EV_INFO << *userVm << '\n';

            // Update the status
            updateVmUserStatus(userVm->getUserId(), userVm->getVmId(), vmFinished);

            // Update priorized
            if (userVm->getBPriorized())
                priorizedHashMap[userVm->getUserId()] = true;

            if (pUserInstance != nullptr)
            {
                emit(responseSignal, pUserInstance->getNId());
                pUserInstance->getInstanceTimesForUpdate().initExec = simTime();
                pUserInstance->setTimeoutMaxSubscribed();
            }
        }
        else
        {
            UserGenerator_simple::handleResponseVmReject(userVm_RAW);
        }
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code or message class?)");
    }

    return pUserInstance;
}

CloudUserInstance *UserGeneratorCost::handleResponseAppTimeout(SIMCAN_Message *msg)
{
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP *>(msg);
    std::string strVmId;

    if (userApp != nullptr)
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

        // Print a debug trace ...
        strVmId = userApp->getVmId();
        EV_INFO << *userApp << '\n';

        pUserInstance = userHashMap.at(userApp->getUserID());
        if (pUserInstance != nullptr)
        {
            emit(failSignal[strVmId], pUserInstance->getNId());

            if (strVmId.compare("") != 0) // Individual VM timeout
            {
                // The next step is to send a subscription to the cloudprovider
                // Recover the user instance, and get the VmRequest

                if (hasToExtendVm(userApp))
                {
                    resumeExecution(userApp);
                    // pUserInstance->setRequestApp(userApp, strVmId);
                    // updateUserApp(userApp);
                } // TODO: Comprobar si ha terminado y hacer cancelAndDeleteMessages (pUserInstace)
                else
                {
                    updateUserApp(userApp);
                    updateVmUserStatus(userApp->getUserID(), strVmId, vmFinished);
                    endExecution(userApp);
                }
            }
        }

        EV_INFO << __func__ << " - End" << endl;
    }
    else
    {
        error("Could not cast SIMCAN_Message to SM_UserAPP (wrong operation code?)");
    }
    // TODO: Mirar cuando eliminar.  delete pUserInstance->getRequestAppMsg();

    return pUserInstance;
}

bool UserGeneratorCost::hasToExtendVm(SM_UserAPP *userApp)
{
    double dRandom;
    CloudUserInstancePriority *pUserInstance;

    pUserInstance = dynamic_cast<CloudUserInstancePriority *>(userHashMap.at(userApp->getUserID()));

    if (pUserInstance == nullptr)
        error("User instance not found or could not cast CloudUserInstance to CloudUserInstancePriority!");

    if (pUserInstance->isBCredit())
        pUserInstance->setBCredit(offerAcceptanceDistribution->doubleValue() <= 0.9);

    return pUserInstance->isBCredit();
}

void UserGeneratorCost::resumeExecution(SM_UserAPP *userApp)
{
    extensionTimeHashMap.at(userApp->getVmId())++;

    EV_INFO << "Sending extend VM rent and resume Apps to the cloud provider" << endl;
    userApp->setIsResponse(false);
    userApp->setOperation(SM_APP_Req_Resume);
    userApp->setResult(0);
    sendRequestMessage(userApp, toCloudProviderGate);
}

void UserGeneratorCost::endExecution(SM_UserAPP *userApp)
{
    EV_INFO << "Sending end VM rent and abort Apps to the cloud provider" << endl;
    userApp->setIsResponse(false);
    userApp->setOperation(SM_APP_Req_End);
    userApp->setResult(0);
    sendRequestMessage(userApp, toCloudProviderGate);
}

SM_UserVM *UserGeneratorCost::createVmMessage()
{
    return new SM_UserVM_Cost();
}

CloudUserInstance *UserGeneratorCost::createCloudUserInstance(CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances)
{
    return new CloudUserInstancePriority(ptrUser, totalUserInstance, userNumber, currentInstanceIndex, totalUserInstances);
}

CloudUser *UserGeneratorCost::createUserTraceType()
{
    return new CloudUserPriority("UserTrace", 0, Regular, strUserTraceSla);
}

void UserGeneratorCost::calculateStatistics()
{
    double dTotalSub, dMeanSub, dNoWaitUsers, dWaitUsers;
    double dUserCost, dTotalCost, dMeanCost, dBaseCost, dRentingBaseCost, dOfferCost, dTotalOfferCost;
    int nIndex, nTotalUnprovided, nTotalRentTimeout, nAcceptOffer, nUsersAcceptOffer, nTotalTimeouts;
    int nPUnprovided, nRUnprovided, nPProvided, nPRProvided, nRProvided;
    std::string strPriorityType;
    Sla::VMCost slaZero = {0.0};
    // UserGenerator_simple::calculateStatistics(); FIXME: Maybe could be fixed or generalized so this could happen

    nIndex = 1;
    nTotalUnprovided = nRUnprovided = nPUnprovided = nTotalRentTimeout = nAcceptOffer = nUsersAcceptOffer = nPProvided = nPRProvided = nRProvided = nTotalTimeouts = 0;
    dTotalSub = dMeanSub = dOfferCost = dTotalOfferCost = 0;
    dTotalCost = 0;
    dNoWaitUsers = dWaitUsers = 0;

    for (const auto &userInstance : userInstances)
    {
        double dSubTime = 0.0;
        // Check the user type
        auto pCloudUser = dataManager->searchUser(userInstance->getType());
        if (pCloudUser == nullptr)
            error("Could not cast CloudUser* to CloudUserPriority* (wrong userType or class?)");

        // Retrieve times and convert them from seconds into hours
        auto times = userInstance->getInstanceTimesForUpdate().convertToSeconds();
        double dInitTime = times.arrival2Cloud.dbl();
        double dEndTime = times.endTime.dbl();
        double dExecTime = times.initExec.dbl();
        double dWaitTime = times.waitTime.dbl();

        double dMaxSub = userInstance->getRentTimes().maxSubscriptionTime.dbl();
        if (dMaxSub != 0)
            dMaxSub = dMaxSub / 3600;

        bool bSubscribed = userInstance->hasSubscribed();
        dBaseCost = dRentingBaseCost = dUserCost = 0;
        bool bUserAcceptOffer = false;

        bool bPriorized = priorizedHashMap.at(userInstance->getId());
        auto pSla = dataManager->searchSla(pCloudUser->getSla());
        auto pUserVM_Rq = userInstance->getRequestVmMsg();

        if (bSubscribed)
        {
            dWaitUsers++;
            if (userInstance->isTimeoutSubscribed())
                dSubTime = dMaxSub;
            else if (dExecTime > dInitTime)
                dSubTime = dExecTime - dInitTime;
            else
                dSubTime = 0;
        }
        else
        {
            dNoWaitUsers++;
            dSubTime = 0;
        }

        dTotalSub += dSubTime;

        for (const auto &vmCollection : userInstance->allVmCollections())
        {
            int nInstances = vmCollection->getNumInstances();
            int nRentTime = vmCollection->getRentTime();

            // Create a loop to insert all the instances.
            auto pvmCost = pSla->getVmCost(vmCollection->getVmType());
            auto &vmCost = pvmCost == nullptr ? *pvmCost : slaZero;
            
            dBaseCost = vmCost.base;

            if (bPriorized)
            {
                if (userInstance->isTimeoutSubscribed())
                {
                    dBaseCost *= -1 * vmCost.compensation;
                }
                else
                {
                    dBaseCost += dBaseCost * vmCost.increase;
                }
            }
            else
            {
                if (userInstance->isTimeoutSubscribed())
                {
                    dBaseCost = 0;
                }
                else if (bSubscribed)
                {
                    dBaseCost -= dBaseCost * vmCost.discount;
                }
            }

            dRentingBaseCost = nRentTime * dBaseCost * nInstances;
            dUserCost += dRentingBaseCost;

            for (const auto &vmInstance : vmCollection->allInstances())
            {
                int nExtendedTime = extensionTimeHashMap.at(vmInstance->getVmInstanceId());
                nAcceptOffer += nExtendedTime;
                if (nExtendedTime > 0)
                {
                    bUserAcceptOffer = true;
                }

                dOfferCost = nExtendedTime * (dBaseCost + dBaseCost * offerCostIncrease);
                dTotalOfferCost += dOfferCost;

                dUserCost += dOfferCost;
            }
        }

        if (bUserAcceptOffer)
            nUsersAcceptOffer++;

        dTotalCost += dUserCost;

        if (pCloudUser->getPriorityType() == Priority)
        {
            if (bPriorized)
            {
                strPriorityType = " -3 ";
            }
            else
            {
                strPriorityType = " -2 ";
            }
        }
        else
        {
            strPriorityType = " -1 ";
        }

        if (userInstance->isTimeoutSubscribed())
        {
            EV_FATAL << "#___#Unprovided " << nIndex << strPriorityType << dSubTime << " " << dWaitTime << "   \n"
                     << endl;
            EV_FATAL << "#___#Compensation " << nIndex << strPriorityType << dUserCost << "   \n"
                     << endl;
            if (bPriorized)
                nPUnprovided++;
            else
                nRUnprovided++;
            nTotalUnprovided++;
        }
        else
        {
            if (pCloudUser->getPriorityType() == Priority)
            {
                if (bPriorized)
                {
                    nPProvided++;
                }
                else
                {
                    nPRProvided++;
                }
            }
            else
            {
                nRProvided++;
            }

            if (userInstance->isTimeoutMaxRent())
            {
                EV_FATAL << "#___#RentTimeout " << nIndex << " " << strPriorityType << dSubTime << " " << dWaitTime << "   \n"
                         << endl;
                EV_FATAL << "#___#RentTimeoutCost " << nIndex << " " << strPriorityType << dUserCost << "   \n"
                         << endl;
                nTotalRentTimeout++;
            }
            else
            {
                EV_FATAL << "#___#Success " << nIndex << " " << strPriorityType << dSubTime << " " << dWaitTime << "   \n"
                         << endl;
                EV_FATAL << "#___#Cost " << nIndex << " " << strPriorityType << dUserCost << "   \n"
                         << endl;
            }
        }
        nIndex++;
    }

    if (userInstances.size() > 0)
    {
        dMeanSub = dTotalSub / userInstances.size();
        dMeanCost = dTotalCost / userInstances.size();
    }

    // Print the experiments data
    EV_FATAL << "#___3d#" << dMeanSub << " \n\n";
    EV_FATAL << "#___3dcost#" << dTotalCost << " \n\n";

    // EV_FATAL << "#___t#" << dMaxSub << " "<< dMeanSub << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided  << " " << dOfferCost << " " << nAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " <<  dWaitUsers << " " <<  dMeanCost << " " <<  dTotalCost << " \n" << endl;
    EV_FATAL << "#___t#" << nPProvided << " " << nPRProvided << " " << nRProvided << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided << " " << dTotalOfferCost << " " << nAcceptOffer << " " << nUsersAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " << dWaitUsers << " "
             << " " << dTotalCost << " \n\n";
}

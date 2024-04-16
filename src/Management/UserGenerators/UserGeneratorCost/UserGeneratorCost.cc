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

using namespace omnetpp;

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

SM_UserVM *UserGeneratorCost::createVmMessage()
{
    return new SM_UserVM_Cost();
}

CloudUserInstance *UserGeneratorCost::createCloudUserInstance(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances)
{
    return new CloudUserInstancePriority(ptrUser, totalUserInstance, userNumber, currentInstanceIndex, totalUserInstances);
}

CloudUser *UserGeneratorCost::createUserTraceType()
{
    return new CloudUserPriority("UserTrace", 0, Regular, strUserTraceSla);
}

void UserGeneratorCost::finish()
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
        // double dEndTime = times.endTime.dbl(); FIXME: Again not used
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

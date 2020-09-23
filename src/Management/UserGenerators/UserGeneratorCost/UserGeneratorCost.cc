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


void UserGeneratorCost::initializeHashMaps() {


    for (int k=0; k<userInstances.size(); k++)
    {
        CloudUserInstance* pUserInstance = userInstances.at(k);
            VmInstanceCollection* pVmCollection;
            //VM_Request* pVmRqArr = &pUserRq->getVms(0);
            int nVmCollections = pUserInstance->getNumberVmCollections();
            std::map<std::string, int> vmExtendedTimes;
            std::string strUserId = pUserInstance->getUserID();

            priorizedHashMap[strUserId] = false;

            for(int j=0; j<nVmCollections; j++)
            {
                VmInstance* pVmInstance;
                pVmCollection = pUserInstance->getVmCollection(j);
                int nVmInstances = pVmCollection->getNumInstances();

                for(int i=0; i<nVmInstances; i++)
                {
                    pVmInstance = pVmCollection->getVmInstance(i);
                    std::string strVmId = pVmInstance->getVmInstanceId();
                    extensionTimeHashMap[strVmId] = 0;
                }

            }
    }
}

CloudUserInstance* UserGeneratorCost::handleResponseAccept(SIMCAN_Message *userVm_RAW)
{
    CloudUserPriority* pCloudUser;
    CloudUserInstance* pUserInstance = nullptr;
    SM_UserVM_Cost *userVm = dynamic_cast<SM_UserVM_Cost*>(userVm_RAW);

    if (userVm == nullptr)
        error("Could not cast SIMCAN_Message to SM_UserVM_Cost (wrong operation code or message class?)");

    pUserInstance = userHashMap.at(userVm->getUserID());

    if (pUserInstance == nullptr)
        error ("User instance not found!");

    pCloudUser = dynamic_cast<CloudUserPriority*>(findUser(pUserInstance->getType()));

    if(pCloudUser == nullptr)
        error("Could not cast CloudUser* to CloudUserPriority* (wrong user id, user type or user class?)");

       //Check the response and proceed with the next action
   if (pCloudUser->getPriorityType() == Priority && userVm->getBPriorized())
       priorizedHashMap[userVm->getUserID()] = true;

    UserGenerator_simple::handleResponseAccept(userVm_RAW);
    
    return pUserInstance;
}

CloudUserInstance* UserGeneratorCost::handleResponseReject(SIMCAN_Message *userVm_RAW) {
    CloudUserPriority* pCloudUser;
    CloudUserInstance* pUserInstance = nullptr;
    SM_UserVM_Cost *userVm = dynamic_cast<SM_UserVM_Cost*>(userVm_RAW);

    if (userVm != nullptr) {

        pUserInstance = userHashMap.at(userVm->getUserID());

        if (pUserInstance == nullptr)
            error ("User instance not found!");

        pCloudUser = dynamic_cast<CloudUserPriority*>(findUser(pUserInstance->getType()));

        //Check the response and proceed with the next action
        if(pCloudUser != nullptr && pCloudUser->getPriorityType() == Priority)
        {
            EV_INFO << __func__ << " - Response message" << endl;

            userVm->printUserVM();

            //Update the status
            updateVmUserStatus(userVm->getUserID(), userVm->getStrVmId(), vmFinished);

            //Update priorized
            if (userVm->getBPriorized())
                priorizedHashMap[userVm->getUserID()] = true;



            if(pUserInstance != nullptr)
            {
                emit(responseSignal, pUserInstance->getId());
                pUserInstance->setInitExecTime(simTime());
                pUserInstance->setTimeoutMaxSubscribed();
            }
        } else {
            UserGenerator_simple::handleResponseReject(userVm_RAW);
        }
    }
    else {
        error("Could not cast SIMCAN_Message to SM_UserVM (wrong operation code or message class?)");
    }

    return pUserInstance;
}

CloudUserInstance* UserGeneratorCost::handleResponseAppTimeout(SIMCAN_Message *msg) {
    CloudUserInstance *pUserInstance = nullptr;
    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP*>(msg);
    std::string strVmId;

    if (userApp != nullptr) {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

        //Print a debug trace ...
        strVmId = userApp->getVmId();
        userApp->printUserAPP();


        pUserInstance = userHashMap.at(userApp->getUserID());
        if (pUserInstance != nullptr)
          {
            emit(failSignal[strVmId], pUserInstance->getId());

            if (strVmId.compare("") != 0) //Individual VM timeout
              {
                //The next step is to send a subscription to the cloudprovider
                //Recover the user instance, and get the VmRequest

                if (hasToExtendVm(userApp))
                  {
                    resumeExecution(userApp);
                    //pUserInstance->setRequestApp(userApp, strVmId);
                    //updateUserApp(userApp);
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
    else {
        error("Could not cast SIMCAN_Message to SM_UserAPP (wrong operation code?)");
    }
    //TODO: Mirar cuando eliminar.  delete pUserInstance->getRequestAppMsg();

    return pUserInstance;
}


void UserGeneratorCost::parseConfig() {
    int result;

        // Init super-class
        cSIMCAN_Core::initialize();

        // Init module parameters
        showApps = par ("showApps");

        // Parse application list
        result = parseAppList();

        // Something goes wrong...
        if (result == SC_ERROR){
            error ("Error while parsing application list.");
        }
        else if (showApps){
            EV_DEBUG << appsToString ();
        }

        // Init module parameters
        showUsersVms = par ("showUsersVms");

        // Parse VMs list
        result = parseVmsList();

        // Something goes wrong...
        if (result == SC_ERROR){
         error ("Error while parsing VMs list");
        }
        else if (showUsersVms){
           EV_DEBUG << vmsToString ();
        }

        // Init module parameters
        showSlas = par ("showSlas");

        // Parse sla list
        result = parseSlasList();

        //Something goes wrong...
        if (result == SC_ERROR){
           error ("Error while parsing slas list");
        }
        else if (showSlas){
           EV_DEBUG << slasToString ();
        }

        // Parse user list
        result = parseUsersList();

        // Something goes wrong...
        if (result == SC_ERROR){
           error ("Error while parsing users list");
        }
        else if (showUsersVms){
           EV_DEBUG << usersToString ();
        }
}

int UserGeneratorCost::parseSlasList (){
    int result;
    const char *slaListChr;

    slaListChr= par ("slaList");
    SlaListParser slaParser(slaListChr, &vmTypes);
    result = slaParser.parse();
    if (result == SC_OK) {
        slaTypes = slaParser.getResult();
    }
    return result;
}

int UserGeneratorCost::parseUsersList (){
    int result;
    const char *userListChr;

    userListChr= par ("userList");
    UserPriorityListParser userParser(userListChr, &vmTypes, &appTypes, &slaTypes);
    result = userParser.parse();
    if (result == SC_OK) {
        userTypes = userParser.getResult();
    }
    return result;
}

bool UserGeneratorCost::hasToExtendVm(SM_UserAPP* userApp)
{
    double dRandom;
    CloudUserInstancePriority* pUserInstance;

    pUserInstance = dynamic_cast<CloudUserInstancePriority*>( userHashMap.at(userApp->getUserID()));

    if (pUserInstance == nullptr)
        error ("User instance not found or could not cast CloudUserInstance to CloudUserInstancePriority!");

    if (pUserInstance->isBCredit())
        pUserInstance->setBCredit(offerAcceptanceDistribution->doubleValue()<=0.9);

    return pUserInstance->isBCredit();
}

void UserGeneratorCost::resumeExecution(SM_UserAPP* userApp)
{
    extensionTimeHashMap.at(userApp->getVmId())++;

    EV_INFO << "Sending extend VM rent and resume Apps to the cloud provider" << endl;
    userApp->setIsResponse(false);
    userApp->setOperation(SM_APP_Req_Resume);
    userApp->setResult(0);
    sendRequestMessage(userApp, toCloudProviderGate);
}

void UserGeneratorCost::endExecution(SM_UserAPP* userApp)
{
    EV_INFO << "Sending end VM rent and abort Apps to the cloud provider" << endl;
    userApp->setIsResponse(false);
    userApp->setOperation(SM_APP_Req_End);
    userApp->setResult(0);
    sendRequestMessage(userApp, toCloudProviderGate);
}

SM_UserVM* UserGeneratorCost::createVmMessage() {
    return new SM_UserVM_Cost();
}

CloudUserInstance* UserGeneratorCost::createCloudUserInstance(CloudUser *ptrUser, unsigned int  totalUserInstance, unsigned int  userNumber, int currentInstanceIndex, int totalUserInstances) {
    return new CloudUserInstancePriority (ptrUser, totalUserInstance, userNumber, currentInstanceIndex, totalUserInstances);
}

//
//SM_UserAPP* UserGeneratorCost::createAppRequest(SM_UserVM* userVm)
//{
//    VM_Response* pRes;
//    VM_Request vmRq;
//    SM_UserAPP* userApp;
//    CloudUserInstance* pUserInstance;
//    AppInstance* pAppInstance;
//    std::string strIp, strAppInstance, strUserName, strService, strAppType, strVmId;
//    int nStartRentTime, nPrice, nMaxStarTime, nIndexRes;
//
//    EV_TRACE << "UserGenerator::createNextAppRequest - Init" << endl;
//
//    if(userVm != nullptr)
//    {
//        userApp = new  SM_UserAPP();
//        strUserName = userVm->getUserID();
//        userApp->setUserID(strUserName.c_str());
//        userApp->setOperation(SM_APP_Req);
//
//        pUserInstance = userHashMap.at(strUserName);
//        pUserInstance->setRequestApp(userApp);
//
//        //Include the Ips and startTime
//        for(int i=0;i<userVm->getTotalVmsRequests();i++)
//        {
//            //Each App to each VM
//            vmRq = userVm->getVms(i);
//
//            for (int nIndexApp=0;pUserInstance->getAppInstance(nIndexApp)!=nullptr; nIndexApp++)
//            {
//                pAppInstance = pUserInstance->getAppInstance(nIndexApp);
//                if(pAppInstance != nullptr)
//                {
//                    strService = pAppInstance->getAppInstanceId();
//                    strAppType = pAppInstance->getAppName();
//                }
//
//                nIndexRes=0;
//
//                if(userVm->hasResponse(i,nIndexRes))
//                {
//                    pRes = userVm->getResponse(i,nIndexRes);
//
//                    if(pRes != nullptr)
//                    {
//                        nMaxStarTime = maxStartTime_t1;
//
//                        //strIp = vmRes.strIp;
//                        //nStartRentTime = vmRes.startTime;
//                        //nPrice = vmRes.nPrice;
//
//                        strIp = pRes->strIp;
//                        nStartRentTime = pRes->startTime;
//                        nPrice = pRes->nPrice;
//                        strVmId = vmRq.strVmId;
//
//                        //TODO: YA funciona esto, adaptar!!
//                        if(bMaxStartTime_t1_active)
//                        {
//                            //Check if T2 <T3
//                            if(nMaxStarTime>=nStartRentTime)
//                            {
//                                userApp->createNewAppRequest(strService, strAppType, strIp, strVmId, nStartRentTime);
//                            }
//                            else
//                            {
//                                //The rent time proposed by the server is too high.
//                                EV_INFO << "The maximum start rent time provided by the cloudprovider is greater than the maximum required by the user: " << nMaxStarTime << " < " << nStartRentTime<< endl;
//                            }
//                        }
//                        else
//                        {
//                            userApp->createNewAppRequest(strService, strAppType, strIp, strVmId, nStartRentTime);
//                        }
//                    }
//
//                }
//                else
//                    EV_INFO << "WARNING! Invalid response in user: " << userVm->getName() << endl;
//            }
//        }
//    }
//
//    EV_TRACE << "UserGenerator::createNextAppRequest - End" << endl;
//
//    return userApp;
//}
//
///*
//void UserGeneratorCost::handleUserAppResponse(SIMCAN_Message* userApp_RAW)
//{
//    SM_UserAPP *userApp = dynamic_cast<SM_UserAPP*>(userApp_RAW);
//    if (userApp == nullptr) {
//        return;
//    }
//
//    //Version hack
//    UserGenerator_simple::handleUserAppResponse(userApp);
//
//    if (false) {
//        EV_INFO << "UserGeneratorCost::handleUserAppResponse - Init" << endl;
//
//        if(userApp->getOperation() == SM_APP_Rsp)
//        {
//            EV_INFO << "UserGeneratorCost::handleUserAppResponse - SM_APP_Rsp" << endl;
//            //Check the response
//            if(userApp->getResult() == SM_APP_Res_Timeout)
//            {
//                EV_INFO << "UserGeneratorCost::handleUserAppResponse - SM_APP_Res_Timeout" << endl;
//                if (userApp->getFinished()) {
//                    UserGenerator_simple::handleUserAppResponse(userApp);
//                }
//                else if(hasToExtendVm(userApp))
//                {
//                    resumeExecution(userApp);
//                }
//                else
//                {
//                    endExecution(userApp);
//                }
//            } else {
//                UserGenerator_simple::handleUserAppResponse(userApp);
//            }
//        }
//        EV_INFO << "UserGeneratorCost::handleUserAppResponse - End" << endl;
//    }
//}
//*/
//
//bool UserGeneratorCost::hasToExtendVm(SM_UserAPP* userApp)
//{
//    double dRandom;
//
//    dRandom = ((double) rand() / (RAND_MAX));
//
//    return dRandom<=offerAcceptanceRate;
//}
//
//void UserGeneratorCost::resumeExecution(SM_UserAPP* userApp)
//{
//    extensionTimeHashMap.at(userApp->getVmId())++;
//
//    EV_INFO << "Sending extend VM rent and resume Apps to the cloud provider" << endl;
//    userApp->setIsResponse(false);
//    userApp->setOperation(SM_APP_Req_Resume);
//    userApp->setResult(0);
//    sendRequestMessage(userApp, toCloudProviderGate);
//}
//
//void UserGeneratorCost::initializeHashMaps() {
//
//
//    for (int k=0; k<userInstances.size(); k++)
//    {
//        CloudUserInstance* pUserInstance = userInstances.at(k);
//            VmInstanceCollection* pVmCollection;
//            //VM_Request* pVmRqArr = &pUserRq->getVms(0);
//            int nVmCollections = pUserInstance->getNumberVmCollections();
//            std::map<std::string, int> vmExtendedTimes;
//            std::string strUserId = pUserInstance->getUserID();
//
//            priorizedHashMap[strUserId] = false;
//
//            for(int j=0; j<nVmCollections; j++)
//            {
//                VmInstance* pVmInstance;
//                pVmCollection = pUserInstance->getVmCollection(j);
//                int nVmInstances = pVmCollection->getNumInstances();
//
//                for(int i=0; i<nVmInstances; i++)
//                {
//                    pVmInstance = pVmCollection->getVmInstance(i);
//                    std::string strVmId = pVmInstance->getVmInstanceId();
//                    extensionTimeHashMap[strVmId] = 0;
//                }
//
//            }
//
////            std::transform(vmInstanceArr, vmInstanceArr+sizeof(vmInstanceArr)*nVmInstances, std::inserter(vmExtendedTimes, vmExtendedTimes.end()),
////                [](VmInstance pVmInstance)
////                {
////                std::string strVmId = pVmInstance.getVmInstanceId();
////                    return std::make_pair(strVmId,0);
////                });
////
//
////            std::transform(pVmRqArr, pVmRqArr+nVmRqs, std::inserter(vmExtendedTimes, vmExtendedTimes.end()),
////                [](const VM_Request pVmRq)
////                {
////                    return std::make_pair(pVmRq.strVmId,0);
////                });
//        }
//
////    std::transform(userInstances.begin(), userInstances.end(), std::inserter(priorizedHashMap, priorizedHashMap.end()),
////        [](CloudUserInstance* pUserInstance)
////        {
////            std::string strUserId = pUserInstance->getUserID();
////            return std::make_pair(strUserId,false);
////        });
//}
//
//void UserGeneratorCost::endExecution(SM_UserAPP* userApp)
//{
//    EV_INFO << "Sending end VM rent and abort Apps to the cloud provider" << endl;
//    userApp->setIsResponse(false);
//    userApp->setOperation(SM_APP_Req_End);
//    userApp->setResult(0);
//    sendRequestMessage(userApp, toCloudProviderGate);
//}
//
//CloudUser* UserGeneratorCost::findUserTypeById (std::string userId){
//
//    std::vector<CloudUser*>::iterator it;
//    CloudUser* result;
//    bool found;
//
//        // Init
//        found = false;
//        result = nullptr;
//        it = userTypes.begin();
//
//        // Search...
//        while((!found) && (it != userTypes.end())){
//
//            if (userId.find((*it)->getType()) != std::string::npos) {
//                found = true;
//                result = (*it);
//            }
//            else
//                it++;
//        }
//
//    return result;
//}
//
///*
//void UserGeneratorCost::handleUserVmResponse(SM_UserVM* userVm) {
//    CloudUser* pCloudUser;
//    CloudUserInstance* pUserInstance;
//    pCloudUser = findUserTypeById(userVm->getUserID());
//
//    //Check the response and proceed with the next action
//    if(userVm->getOperation() == SM_VM_Req_Rsp && userVm->getResult() == SM_VM_Res_Reject && pCloudUser->getPriorityType() == Priority)
//    {
//        userVm->setOperation(SM_VM_Notify);
//        userVm->setResult(SM_APP_Sub_Timeout);
//        pUserInstance = userHashMap.at(userVm->getUserID());
//
//        if(pUserInstance != nullptr)
//        {
//            pUserInstance->setSubscribe(false);
//        }
//    }
//
//    UserGenerator_simple::handleUserVmResponse(userVm);
//
//}
//*/
//
//SM_UserVM* UserGeneratorCost::createVmRequest(CloudUserInstance* pUserInstance)
//{
//    int nVmIndex, nCollectionNumber, nInstances;
//    double dRentTime;
//    std::string userId, userType, vmType, instanceId;
//    SM_UserVM_Cost* pUserRet;
//    VmInstance* pVmInstance;
//    VmInstanceCollection* pVmCollection;
//
//    EV_TRACE << "UserGenerator::createNextVmRequest - Init" << endl;
//
//    pUserRet = nullptr;
//
//    nVmIndex = 0;
//
//    if(pUserInstance != nullptr)
//    {
//        pUserInstance->setRentTimes(maxStartTime_t1, nRentTime_t2, maxSubTime_t3, maxSubscriptionTime_t4);
//        nCollectionNumber = pUserInstance->getNumberVmCollections();
//        userId = pUserInstance->getUserID();
//        userType = pUserInstance->getType();
//
//        EV_TRACE << "UserGenerator::createNextVmRequest - UserId: " << userId << " | maxStartTime_t1: "<< maxStartTime_t1 <<
//                " | rentTime_t2: " << nRentTime_t2 <<" | maxSubTime: " <<maxSubTime_t3<< " | MaxSubscriptionTime_T4:" << maxSubscriptionTime_t4<< endl;
//
//        if(nCollectionNumber>0 && userId.size()>0)
//        {
//            //Creation of the message
//            pUserRet = new SM_UserVM_Cost();
//            pUserRet->setUserID(userId.c_str());
//            pUserRet->setStrUserType(userType.c_str());
//            pUserRet->setIsResponse(false);
//            pUserRet->setOperation(SM_VM_Req);
//            pUserRet->setBPriorized(false);
//
//            //Get all the collections and all the instances!
//            for(int i=0; i<nCollectionNumber;i++)
//            {
//                pVmCollection = pUserInstance->getVmCollection(i);
//                if(pVmCollection)
//                {
//                    nInstances = pVmCollection->getNumInstances();
//                    dRentTime = pVmCollection->getRentTime()*3600;
//                    //Create a loop to insert all the instances.
//                    vmType = pVmCollection->getVmType();
//                    for(int j=0;j<nInstances;j++)
//                    {
//                        pVmInstance = pVmCollection->getVmInstance(j);
//                        if(pVmInstance!=NULL)
//                        {
//                            instanceId =pVmInstance->getVmInstanceId();
//                            pUserRet->createNewVmRequest(vmType, instanceId, maxStartTime_t1, dRentTime, maxSubTime_t3, maxSubscriptionTime_t4);
//                        }
//                    }
//                }
//                else
//                {
//                    EV_TRACE << "WARNING! [UserGenerator] The VM collection is empty" << endl;
//                    throw omnetpp::cRuntimeError("[UserGenerator] The VM collection is empty!");
//                }
//            }
//        }
//        else
//        {
//            EV_TRACE << "WARNING! [UserGenerator] Collection or User-ID malformed" << endl;
//            throw omnetpp::cRuntimeError("[UserGenerator] Collection or User-ID malformed!");
//        }
//    }
//    else
//    {
//        EV_INFO << "UserGenerator::createNextVmRequest - The user instance is null" << endl;
//    }
//
//    EV_TRACE << "UserGenerator::createNextVmRequest - End" << endl;
//
//    return pUserRet;
//}
//
//void UserGeneratorCost::calculateStatistics() {
//    SimTime dInitTime, dEndTime, dExecTime, dSubTime, dMaxSub, dTotalSub,  dMeanSub;
//    double dUserCost, dTotalCost, dMeanCost, dBaseCost, dRentingBaseCost, dOfferCost, dTotalOfferCost, dNoWaitUsers, dWaitUsers;
//    int nIndex, nSize, nTotalUnprovided, nTotalRentTimeout, nRentTime, nAcceptOffer;
//    CloudUserInstance* pUserInstance;
//    CloudUserPriority* pCloudUser;
//    Sla* pSla;
//    std::vector <CloudUserInstance*> userVector;
//    SM_UserVM* pUserVM_Rq;
//    VM_Request vmRequest;
//    Sla::VMCost vmCost;
//    VmInstanceCollection* pVmCollection;
//    VmInstance* pVmInstance;
//    int nRequestedVms, nExtendedTime, nCollectionNumber, nInstances, nPUnprovided, nRUnprovided, nPProvided, nPRProvided, nRProvided;
//    bool bSubscribed, bPriorized;
//    std::string strUserType, strUserId, strVmType, strVmId, strPriorityType;
//
//    //UserGenerator_simple::calculateStatistics();
//
//    nIndex=1;
//    nRentTime = nTotalUnprovided=nRUnprovided=nPUnprovided=nTotalRentTimeout=nAcceptOffer=nPProvided=nPRProvided=nRProvided=0;
//    dInitTime = dEndTime = dExecTime = dSubTime = dMaxSub = dTotalSub =  dMeanSub = dOfferCost = dTotalOfferCost = 0;
//    dTotalCost = 0;
//    dNoWaitUsers= dWaitUsers =0;
//    nRequestedVms = 0;
//    bSubscribed = false;
//    nSize = userInstances.size();
//
//    for(int i=0;(i<nSize);i++)
//    {
//        pUserInstance = userInstances.at(i);
//        dInitTime = pUserInstance->getArrival2Cloud();
//        dEndTime = pUserInstance->getEndTime();
//        dMaxSub = pUserInstance->getT4();
//        dExecTime = pUserInstance->getInitExecTime();
//        bSubscribed = pUserInstance->hasSubscribed();
//        strUserType = pUserInstance->getType();
//        strUserId = pUserInstance->getUserID();
//        dBaseCost = dRentingBaseCost = dUserCost = 0;
//
//        bPriorized = priorizedHashMap.at(strUserId);
//
//        pCloudUser = dynamic_cast<CloudUserPriority*>(findUser(strUserType));
//        if (pCloudUser!=nullptr)
//            pSla = pCloudUser->getSla();
//
//        pUserVM_Rq = pUserInstance->getRequestVmMsg();
//
////        nRequestedVms = pUserVM_Rq->getTotalVmsRequests();
//
//
//        if(dMaxSub!=0)
//            dMaxSub = dMaxSub/3600;
//
//        if(dInitTime!=0)
//            dInitTime = dInitTime/3600;
//
//        if(dEndTime!=0)
//            dEndTime = dEndTime/3600;
//
//        if(dExecTime!=0)
//            dExecTime = dExecTime/3600;
//
//        if(bSubscribed)
//        {
//            dWaitUsers++;
//            if(pUserInstance->isTimeoutSubscribed())
//                dSubTime = dMaxSub;
//            else if(dExecTime > dInitTime)
//                dSubTime = dExecTime - dInitTime;
//            else
//                dSubTime = 0;
//        }
//        else
//        {
//            dNoWaitUsers++;
//            dSubTime = 0;
//        }
//
//        dTotalSub+=dSubTime;
//
//        nCollectionNumber = pUserInstance->getNumberVmCollections();
//        for(int i=0; i<nCollectionNumber;i++)
//        {
//            pVmCollection = pUserInstance->getVmCollection(i);
//            if(pVmCollection!=nullptr)
//            {
//                nInstances = pVmCollection->getNumInstances();
//                nRentTime = pVmCollection->getRentTime();
//                //Create a loop to insert all the instances.
//                strVmType = pVmCollection->getVmType();
//                vmCost = pSla->getVmCost(strVmType);
//
//                dBaseCost = vmCost.base;
//
//                if(bPriorized)
//                {
//                    if (pUserInstance->isTimeoutSubscribed())
//                    {
//                        dBaseCost *= -1*vmCost.compensation;
//                    }
//                    else
//                    {
//                        dBaseCost += dBaseCost*vmCost.increase;
//                    }
//                } else {
//                    if(pUserInstance->isTimeoutSubscribed())
//                    {
//                        dBaseCost = 0;
//                    }
//                    else if(bSubscribed)
//                    {
//                        dBaseCost-=dBaseCost*vmCost.discount;
//
//                    }
//                }
//
//                dRentingBaseCost = nRentTime*dBaseCost*nInstances;
//                dUserCost+=dRentingBaseCost;
//
//                for(int j=0;j<nInstances;j++)
//                {
//                    pVmInstance = pVmCollection->getVmInstance(j);
//                    if(pVmInstance!=NULL)
//                    {
//                        strVmId =pVmInstance->getVmInstanceId();
//
//                        nExtendedTime = extensionTimeHashMap.at(strVmId);
//                        nAcceptOffer += nExtendedTime;
//
//                        dOfferCost = nExtendedTime*(dBaseCost+dBaseCost*offerCostIncrease);
//                        dTotalOfferCost+=dOfferCost;
//
//                        dUserCost+= dOfferCost;
//                    }
//                }
//            }
//        }
//
//        dTotalCost+=dUserCost;
//
//        if (pCloudUser->getPriorityType() == Priority)
//        {
//            if (bPriorized)
//            {
//                strPriorityType = " -3 ";
//            }
//            else
//            {
//                strPriorityType = " -2 ";
//            }
//        }
//        else
//        {
//            strPriorityType = " -1 ";
//        }
//
//        if(pUserInstance->isTimeoutSubscribed())
//        {
//            EV_FATAL << "#___#Unprovided " << nIndex << strPriorityType << dSubTime <<"   \n" << endl;
//            EV_FATAL << "#___#Compensation " << nIndex << strPriorityType << dUserCost <<"   \n" << endl;
//            if (bPriorized)
//                nPUnprovided++;
//            else
//                nRUnprovided++;
//            nTotalUnprovided++;
//        }
//        else
//        {
//            if (pCloudUser->getPriorityType() == Priority)
//            {
//                if (bPriorized)
//                {
//                    //strPriorityType = " -3 ";
//                    nPProvided++;
//                }
//                else
//                {
//                    //strPriorityType = " -2 ";
//                    nPRProvided++;
//                }
//            }
//            else
//            {
//                //strPriorityType = " -1 ";
//                nRProvided++;
//            }
//
//            if (pUserInstance->isTimeoutMaxRent())
//            {
//                EV_FATAL << "#___#RentTimeout " << nIndex << " " << dSubTime << strPriorityType <<"   \n" << endl;
//                EV_FATAL << "#___#RentTimeoutCost " << nIndex << " "  << dUserCost << strPriorityType <<"   \n" << endl;
//                nTotalRentTimeout++;
//            }
//            else
//            {
//                EV_FATAL << "#___#Success " << nIndex << " "<< dSubTime << strPriorityType <<"   \n" << endl;
//                EV_FATAL << "#___#Cost " << nIndex << " "<< dUserCost << strPriorityType <<"   \n" << endl;
//            }
//        }
//        nIndex++;
//    }
//
//    if(nSize >0)
//    {
//        dMeanSub = dTotalSub/nSize;
//        dMeanCost = dTotalCost/nSize;
//    }
//
//    //Print the experiments data
//    EV_FATAL << "#___3d#" << dMeanSub <<" \n" << endl;
//    EV_FATAL << "#___3dcost#" << dTotalCost <<" \n" << endl;
//    //EV_FATAL << "#___t#" << dMaxSub << " "<< dMeanSub << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided  << " " << dOfferCost << " " << nAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " <<  dWaitUsers << " " <<  dMeanCost << " " <<  dTotalCost << " \n" << endl;
//    EV_FATAL << "#___t#" << nPProvided << " " << nPRProvided << " " << nRProvided << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided  << " " << dTotalOfferCost << " " << nAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " <<  dWaitUsers << " " << " " <<  dTotalCost << " \n" << endl;
//}
//
//void UserGeneratorCost::updateVmUserStatus(std::string strUserId, std::string strVmId, tVmState state) {
//    SM_UserVM_Cost* userVmCost;
//
//    UserGenerator_simple::updateVmUserStatus(strUserId, strVmId, state);
//
//    /*
//    if ((userVmCost = dynamic_cast<SM_UserVM_Cost*>(userVm)))
//      {
//        strUserId = userVmCost->getUserID();
//
//        if(userVmCost->getBPriorized())
//            priorizedHashMap.at(strUserId)=true;
//      }
//      */
//}
std::string UserGeneratorCost::slasToString (){

    std::ostringstream info;
    int i;

        // Main text for the users of this manager
        info << std::endl << slaTypes.size() << " Slas parsed from ManagerBase in " << getFullPath() << endl << endl;

        for (i=0; i<slaTypes.size(); i++){
            info << "\tSla[" << i << "]  --> " << slaTypes.at(i)->toString() << endl;
        }

        info << "---------------- End of parsed Slas in " << getFullPath() << " ----------------" << endl;

    return info.str();
}

Sla* UserGeneratorCost::findSla (std::string slaType){

    std::vector<Sla*>::iterator it;
    Sla* result;
    bool found;

        // Init
        found = false;
        result = nullptr;
        it = slaTypes.begin();

        // Search...
        while((!found) && (it != slaTypes.end())){

            if ((*it)->getType() == slaType){
                found = true;
                result = (*it);
            }
            else
                it++;
        }

    return result;
}


void UserGeneratorCost::calculateStatistics() {
    double dInitTime, dEndTime, dExecTime, dSubTime, dMaxSub, dTotalSub,  dMeanSub, dNoWaitUsers, dWaitUsers;
    double dUserCost, dTotalCost, dMeanCost, dBaseCost, dRentingBaseCost, dOfferCost, dTotalOfferCost;
    int nIndex, nSize, nTotalUnprovided, nTotalRentTimeout, nRentTime, nAcceptOffer, nUsersAcceptOffer;
    CloudUserInstance* pUserInstance;
    CloudUserPriority* pCloudUser;
    Sla* pSla;
    std::vector <CloudUserInstance*> userVector;
    SM_UserVM* pUserVM_Rq;
    VM_Request vmRequest;
    Sla::VMCost vmCost;
    VmInstanceCollection* pVmCollection;
    VmInstance* pVmInstance;
    int nRequestedVms, nExtendedTime, nCollectionNumber, nInstances, nPUnprovided, nRUnprovided, nPProvided, nPRProvided, nRProvided;
    bool bSubscribed, bPriorized, bUserAcceptOffer;
    std::string strUserType, strUserId, strVmType, strVmId, strPriorityType;

    //UserGenerator_simple::calculateStatistics();

    nIndex=1;
    nRentTime = nTotalUnprovided=nRUnprovided=nPUnprovided=nTotalRentTimeout=nAcceptOffer=nUsersAcceptOffer=nPProvided=nPRProvided=nRProvided=0;
    dInitTime = dEndTime = dExecTime = dSubTime = dMaxSub = dTotalSub =  dMeanSub = dOfferCost = dTotalOfferCost = 0;
    dTotalCost = 0;
    dNoWaitUsers= dWaitUsers =0;
    nRequestedVms = 0;
    bSubscribed = false;
    nSize = userInstances.size();

    for(int i=0;(i<nSize);i++)
    {
        pUserInstance = userInstances.at(i);
        dInitTime = pUserInstance->getArrival2Cloud().dbl();
        dEndTime = pUserInstance->getEndTime().dbl();
        dMaxSub = pUserInstance->getT4();
        dExecTime = pUserInstance->getInitExecTime().dbl();
        bSubscribed = pUserInstance->hasSubscribed();
        strUserType = pUserInstance->getType();
        strUserId = pUserInstance->getUserID();
        dBaseCost = dRentingBaseCost = dUserCost = 0;
        bUserAcceptOffer=false;

        bPriorized = priorizedHashMap.at(strUserId);

        pCloudUser = dynamic_cast<CloudUserPriority*>(findUser(strUserType));

        if (pCloudUser == nullptr)
            error("Could not cast CloudUser* to CloudUserPriority* (wrong userType or class?)");

        pSla = pCloudUser->getSla();

        pUserVM_Rq = pUserInstance->getRequestVmMsg();

//        nRequestedVms = pUserVM_Rq->getTotalVmsRequests();


        if(dMaxSub!=0)
            dMaxSub = dMaxSub/3600;

        if(dInitTime!=0)
            dInitTime = dInitTime/3600;

        if(dEndTime!=0)
            dEndTime = dEndTime/3600;

        if(dExecTime!=0)
            dExecTime = dExecTime/3600;

        if(bSubscribed)
        {
            dWaitUsers++;
            if(pUserInstance->isTimeoutSubscribed())
                dSubTime = dMaxSub;
            else if(dExecTime > dInitTime)
                dSubTime = dExecTime - dInitTime;
            else
                dSubTime = 0;
        }
        else
        {
            dNoWaitUsers++;
            dSubTime = 0;
        }

        dTotalSub+=dSubTime;

        nCollectionNumber = pUserInstance->getNumberVmCollections();
        for(int i=0; i<nCollectionNumber;i++)
        {
            pVmCollection = pUserInstance->getVmCollection(i);
            if(pVmCollection!=nullptr)
            {
                nInstances = pVmCollection->getNumInstances();
                nRentTime = pVmCollection->getRentTime();
                //Create a loop to insert all the instances.
                strVmType = pVmCollection->getVmType();
                vmCost = pSla->getVmCost(strVmType);

                dBaseCost = vmCost.base;

                if(bPriorized)
                {
                    if (pUserInstance->isTimeoutSubscribed())
                    {
                        dBaseCost *= -1*vmCost.compensation;
                    }
                    else
                    {
                        dBaseCost += dBaseCost*vmCost.increase;
                    }
                } else {
                    if(pUserInstance->isTimeoutSubscribed())
                    {
                        dBaseCost = 0;
                    }
                    else if(bSubscribed)
                    {
                        dBaseCost-=dBaseCost*vmCost.discount;

                    }
                }

                dRentingBaseCost = nRentTime*dBaseCost*nInstances;
                dUserCost+=dRentingBaseCost;

                for(int j=0;j<nInstances;j++)
                {
                    pVmInstance = pVmCollection->getVmInstance(j);
                    if(pVmInstance!=NULL)
                    {
                        strVmId =pVmInstance->getVmInstanceId();

                        nExtendedTime = extensionTimeHashMap.at(strVmId);
                        nAcceptOffer += nExtendedTime;
                        if (nExtendedTime>0)
                        {
                            bUserAcceptOffer=true;
                        }

                        dOfferCost = nExtendedTime*(dBaseCost+dBaseCost*offerCostIncrease);
                        dTotalOfferCost+=dOfferCost;

                        dUserCost+= dOfferCost;
                    }
                }
            }
        }

        if(bUserAcceptOffer) nUsersAcceptOffer++;

        dTotalCost+=dUserCost;

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

        if(pUserInstance->isTimeoutSubscribed())
        {
            EV_FATAL << "#___#Unprovided " << nIndex << strPriorityType << dSubTime <<"   \n" << endl;
            EV_FATAL << "#___#Compensation " << nIndex << strPriorityType << dUserCost <<"   \n" << endl;
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
                    //strPriorityType = " -3 ";
                    nPProvided++;
                }
                else
                {
                    //strPriorityType = " -2 ";
                    nPRProvided++;
                }
            }
            else
            {
                //strPriorityType = " -1 ";
                nRProvided++;
            }

            if (pUserInstance->isTimeoutMaxRent())
            {
                EV_FATAL << "#___#RentTimeout " << nIndex << " " << dSubTime << strPriorityType <<"   \n" << endl;
                EV_FATAL << "#___#RentTimeoutCost " << nIndex << " "  << dUserCost << strPriorityType <<"   \n" << endl;
                nTotalRentTimeout++;
            }
            else
            {
                EV_FATAL << "#___#Success " << nIndex << " "<< dSubTime << strPriorityType <<"   \n" << endl;
                EV_FATAL << "#___#Cost " << nIndex << " "<< dUserCost << strPriorityType <<"   \n" << endl;
            }
        }
        nIndex++;
    }

    if(nSize >0)
    {
        dMeanSub = dTotalSub/nSize;
        dMeanCost = dTotalCost/nSize;
    }

    //Print the experiments data
    EV_FATAL << "#___3d#" << dMeanSub <<" \n" << endl;
    EV_FATAL << "#___3dcost#" << dTotalCost <<" \n" << endl;
    //EV_FATAL << "#___t#" << dMaxSub << " "<< dMeanSub << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided  << " " << dOfferCost << " " << nAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " <<  dWaitUsers << " " <<  dMeanCost << " " <<  dTotalCost << " \n" << endl;
    EV_FATAL << "#___t#" << nPProvided << " " << nPRProvided << " " << nRProvided << " " << nTotalUnprovided << " " << nRUnprovided << " " << nPUnprovided  << " " << dTotalOfferCost << " " << nAcceptOffer << " " << nUsersAcceptOffer << " " << nTotalRentTimeout << " " << dNoWaitUsers << " " <<  dWaitUsers << " " << " " <<  dTotalCost << " \n" << endl;
}

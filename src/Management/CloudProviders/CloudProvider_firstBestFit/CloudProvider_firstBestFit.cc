#include "CloudProvider_firstBestFit.h"
#include "Management/utils/LogUtils.h"

Define_Module(CloudProvider_firstBestFit);

CloudProvider_firstBestFit::~CloudProvider_firstBestFit(){
}


void CloudProvider_firstBestFit::initialize(){
    int nIndex;

    EV_INFO << "CloudProviderFirstFit::initialize - Init" << endl;

    // Init super-class
    CloudProviderBase_firstBestFit::initialize();
}

void CloudProvider_firstBestFit::initializeDataCenterCollection(){
    datacenterCollection = new DataCenterInfoCollectionReservation();
}

void CloudProvider_firstBestFit::initializeRequestHandlers() {
    CloudProviderBase_firstBestFit::initializeRequestHandlers();

    requestHandlers[SM_APP_Req_Resume] = [this](SIMCAN_Message *msg) -> void { return handleExtendVmAndResumeExecution(msg); };
    requestHandlers[SM_APP_Req_End] = [this](SIMCAN_Message *msg) -> void { return handleEndVmAndAbortExecution(msg); };
}

void CloudProvider_firstBestFit::parseConfig() {
    int result;

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

int CloudProvider_firstBestFit::parseSlasList (){
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

int CloudProvider_firstBestFit::parseUsersList (){
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

int CloudProvider_firstBestFit::parseDataCentersList (){
    int result;
    const char *dataCentersListChr;

    dataCentersListChr= par ("dataCentersList");
    DataCenterReservationListParser dataCenterParser(dataCentersListChr);
    result = dataCenterParser.parse();
    if (result == SC_OK) {
        dataCentersBase = dataCenterParser.getResult();
    }
    return result;
}

void CloudProvider_firstBestFit::loadNodes(){
    int nIndex;
    std::vector<int> reservedNodes;
    std::vector<int>::iterator it;

    it = reservedNodes.begin();

    for (nIndex=0; nIndex<dataCentersBase.size(); nIndex++){
        DataCenterReservation* dataCenterReservation = dynamic_cast<DataCenterReservation*>(dataCentersBase.at(nIndex));
        if (dataCenterReservation != nullptr)
        {

            reservedNodes.insert(it+nIndex, dataCenterReservation->getReservedNodes());
        }
    }

    DataCenterInfoCollectionReservation* datacenterCollectionReservation = dynamic_cast<DataCenterInfoCollectionReservation*>(datacenterCollection);

    if (datacenterCollectionReservation != nullptr)
    {
        datacenterCollectionReservation->setReservedNodes(reservedNodes);
    }

    CloudProviderBase_firstBestFit::loadNodes();
}

void CloudProvider_firstBestFit::handleExecVmRentTimeout(cMessage *msg) {
    SM_UserAPP *pUserApp;

    std::string strUsername,
                strVmId,
                strAppName,
                strIp;

    SM_UserVM_Finish *pUserVmFinish;

    std::map<std::string, SM_UserAPP*>::iterator it;

    if ((pUserVmFinish = dynamic_cast<SM_UserVM_Finish*>(msg)))
      {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - INIT" << endl;
        strVmId = pUserVmFinish->getStrVmId();


            strUsername = pUserVmFinish->getUserID();
            EV_INFO << "The rent of the VM [" << strVmId
                                   << "] launched by the user " << strUsername
                                   << " has finished" << endl;
            //Check the Application status
            it = handlingAppsRqMap.find(strUsername);
            if (it != handlingAppsRqMap.end())
              {
                pUserApp = it->second;

                //Check the application status
                if (pUserApp != nullptr)
                  {

                    EV_INFO << "Last id gate: " << pUserApp->getLastGateId() << endl;
                    EV_INFO
                    << "Checking the status of the applications which are running over this VM"
                    << endl;

                    //Abort the running applications
                    if (!pUserApp->allAppsFinished(strVmId))
                      {
                        EV_INFO << "Aborting running applications" << endl;
                        abortAllApps(pUserApp, strVmId);
                      }
                    else
                    {
                        EV_INFO << "Freeing resources..." << endl;

                        //Free the VM resources
                        freeVm(strVmId);

                        //Check the subscription queue
                        updateSubsQueue();
                    }
                    // Check the result and send it
                    checkAllAppsFinished(pUserApp, strVmId);

                  }
              }


      }
    else
      {
        error ("%s - Unable to cast msg to SM_UserVM_Finish*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), msg->getName());
      }
}

void CloudProvider_firstBestFit::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP*>(sm);

    if (userAPP_Rq != nullptr) {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage (sm, toDataCenterGates[0]);
    }
    else
    {
      error ("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }


        //TODO: Select datacenter

//    string strVmId, strUsername;
//    SM_UserAPP *userAPP_Rq;
//    SM_UserVM* pUserVmRequest;
//    std::map<std::string, SM_UserVM*>::iterator it;
//    userAPP_Rq = dynamic_cast<SM_UserAPP*>(sm);
//    bool bFound;
//
//    if (userAPP_Rq != nullptr) {
//        strVmId = userAPP_Rq->getVmId();
//        if (!strVmId.empty()) {
//            strUsername = userAPP_Rq->getUserID();
//            it = acceptedUsersRqMap.find(strUsername);
//
//            if(it != acceptedUsersRqMap.end())
//              {
//                pUserVmRequest = it->second;
//                bFound = false;
//                for(int j = 0; j < pUserVmRequest->getVmsArraySize() && !bFound; j++)
//                  {
//                    //Getting VM and scheduling renting timeout
//                    VM_Request& vmRequest = pUserVmRequest->getVms(j);
//                    //scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUsername, vmRequest.strVmId, vmRequest.nRentTime_t2);
//
//                    if (strVmId.compare(vmRequest.strVmId) == 0) {
//                        bFound = true;
//                        vmRequest.pMsg = scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUsername, strVmId, 3600);
//                        handleUserAppRequest(sm);
//                    }
//
//                  }
//
//              }
//            else
//              {
//                EV_INFO << "WARNING! [" << LogUtils::prettyFunc(__FILE__, __func__) << "] The user: " << strUsername << "has not previously registered!!"<< endl;
//              }
//        }
//    }
//    else
//  {
//    error ("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
//  }
}

void CloudProvider_firstBestFit::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP*>(sm);

    if (userAPP_Rq != nullptr) {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage (sm, toDataCenterGates[0]);
    }
    else
    {
      error ("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }

}


////ToDo: Comprueba primero con el método del padre y luego si es prioritario. Es muy hardcoded. Se puede separar la comprobación de los prioritarios,
////en otro metódo y entonces es solo llamar a cada una de las funciones dependiendo de lo que necesites. Y se debe añadir un parámetro configurable
////desde el ini para indicar que comprueba primero. Ya que tenemos una lista con los tipos de máquinas, ahora solo Normal o Reserved, pero en un
////futuro podrían ser más, se podría hacer que para cada tipo de usuario se indique en una lista dónde puede ser alojado y en que orden.
////Seguramente se pueda separar el método en varios ya que es muy largo y seguro que hay código repetido de otras funciones.
//bool CloudProvider_firstBestFit::checkVmUserFit(SM_UserVM*& userVM_Rq)
//{
//
//    bool bRet, bAccepted;
//
//    int nTotalRequestedCores, nRequestedVms, nAvailableReservedCores,
//            nTotalReservedCores;
//
//    std::string nodeIp, strUserName, strVmId;
//
//    CloudUserPriority *pCloudUser;
//    SM_UserVM_Cost *userVm_Rq_Cost;
//
//    bRet = false;
//    if (userVM_Rq != nullptr) {
//
//        if (!checkReservedFirst) {
//            bRet = CloudProviderBase_firstBestFit::checkVmUserFit(userVM_Rq);
//        }
//
//        if (!bRet) {
//
//            strUserName = userVM_Rq->getUserID();
//
//            pCloudUser = dynamic_cast<CloudUserPriority*>(findUserTypeById(strUserName));
//
//            if (pCloudUser != nullptr &&  pCloudUser->getPriorityType() == Priority) {
//
//                DataCenterInfoCollectionReservation *datacenterCollectionReservation =
//                        dynamic_cast<DataCenterInfoCollectionReservation*>(datacenterCollection);
//
//                bAccepted = bRet = true;
//                if (datacenterCollectionReservation != nullptr) {
//                    nRequestedVms = userVM_Rq->getTotalVmsRequests();
//
//                    EV_DEBUG << "checkVmUserFit - Init - in reserved nodes "
//                                    << endl;
//                    EV_DEBUG
//                                    << "checkVmUserFit - checking for free space in reserved nodes, "
//                                    << nRequestedVms << " Vm(s) for the user"
//                                    << strUserName << endl;
//
//                    nAvailableReservedCores =
//                            datacenterCollectionReservation->getTotalReservedAvailableCores();
//                    nTotalRequestedCores = calculateTotalCoresRequested(
//                            userVM_Rq);
//
//                    if (nTotalRequestedCores <= nAvailableReservedCores) {
//                        nTotalReservedCores =
//                                datacenterCollectionReservation->getTotalReservedCores();
//                        EV_DEBUG
//                                        << "checkVmUserFit - There is available space in reserved nodes: ["
//                                        << strUserName << nTotalRequestedCores
//                                        << " vs Available ["
//                                        << nAvailableReservedCores << "/"
//                                        << nTotalRequestedCores << "]" << endl;
//
//                        //Process all the VMs
//                        for (int i = 0; i < nRequestedVms && bRet; i++) {
//                            EV_DEBUG << endl
//                                            << "checkVmUserFit - Trying to handle the VM: "
//                                            << i << " in reserved node" << endl;
//
//                            //Get the VM request
//                            VM_Request &vmRequest = userVM_Rq->getVms(i);
//
//                            //Create and fill the noderesource  with the VMrequest
//                            NodeResourceRequest *pNode = generateNode(
//                                    strUserName, vmRequest);
//
//                            //Send the request to the DC
//                            bAccepted =
//                                    datacenterCollectionReservation->handlePrioritaryVmRequest(
//                                            pNode);
//
//                            //We need to know the price of the Node.
//                            userVM_Rq->createResponse(i, bAccepted,
//                                    pNode->getStartTime(), pNode->getIp(),
//                                    pNode->getPrice());
//                            bRet &= bAccepted;
//
//                            if (!bRet) {
//                                clearVMReq(userVM_Rq, i);
//                                EV_DEBUG << "checkVmUserFit - The VM: " << i
//                                                << "has not been handled, not enough space in reserved nodes, all the request of the user "
//                                                << strUserName
//                                                << "have been deleted" << endl;
//                            } else {
//                                //Getting VM and scheduling renting timeout
//                                vmRequest.pMsg = scheduleRentingTimeout(
//                                        EXEC_VM_RENT_TIMEOUT, strUserName,
//                                        vmRequest.strVmId,
//                                        vmRequest.nRentTime_t2);
//
//                                //Update value
//                                nAvailableReservedCores =
//                                        datacenterCollectionReservation->getTotalReservedAvailableCores();
//                                EV_DEBUG << "checkVmUserFit - The VM: " << i
//                                                << " has been handled and stored sucessfully in reserved node, available cores: "
//                                                << nAvailableReservedCores
//                                                << endl;
//                            }
//                        }
//
//                        if (bRet) {
//                            userVm_Rq_Cost = dynamic_cast<SM_UserVM_Cost*>(userVM_Rq);
//                            if (userVm_Rq_Cost == nullptr)
//                                error("Could not cast SIMCAN_Message to SM_UserVM_Cost (wrong operation code or message class?)");
//                            userVm_Rq_Cost->setBPriorized(true);
//                        }
//                        //Update the data
//                        nAvailableReservedCores =
//                                datacenterCollectionReservation->getTotalReservedAvailableCores();
//                        nTotalReservedCores = datacenterCollectionReservation->getTotalReservedCores();
//
//                        EV_DEBUG << "checkVmUserFit - Updated space in reserved nodes#: ["
//                                        << strUserName
//                                        << "Requested: " << nTotalRequestedCores
//                                        << " vs Available [" << nAvailableReservedCores
//                                        << "/" << nTotalReservedCores << "]" << endl;
//                    } else {
//                        EV_DEBUG
//                                        << "checkVmUserFit - There isnt enough space in reserved nodes: ["
//                                        << strUserName
//                                        << nTotalRequestedCores << " vs "
//                                        << nAvailableReservedCores << endl;
//                        bRet = false;
//                    }
//
//                }
//                else
//                {
//                    EV_ERROR << "checkVmUserFit - WARNING!! nullpointer detected" <<endl;
//                    bRet = false;
//                }
//
//            }
//            else
//            {
//                EV_DEBUG << "checkVmUserFit - User is not prioritary: " << strUserName << endl;
//                bRet = false;
//            }
//        }
//
//        if (bRet)
//            EV_DEBUG << "checkVmUserFit - Reserved space for: "
//                            << userVM_Rq->getUserID() << endl;
//        else
//            EV_DEBUG << "checkVmUserFit - Unable to reserve space in reserved nodes for: "
//                            << userVM_Rq->getUserID() << endl;
//
//        if (!bRet && checkReservedFirst) {
//            bRet = CloudProviderBase_firstBestFit::checkVmUserFit(userVM_Rq);
//
//        }
//
//        if (!bRet && pCloudUser->getPriorityType() == Priority) {
//            userVm_Rq_Cost = dynamic_cast<SM_UserVM_Cost*>(userVM_Rq);
//            if (userVm_Rq_Cost != nullptr)
//                userVm_Rq_Cost->setBPriorized(true);
//        }
//    } else {
//        EV_ERROR << "checkVmUserFit - WARNING!! nullpointer detected" << endl;
//        bRet = false;
//    }
//
//    return bRet;
//}



std::string CloudProvider_firstBestFit::slasToString (){

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

Sla* CloudProvider_firstBestFit::findSla (std::string slaType){

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

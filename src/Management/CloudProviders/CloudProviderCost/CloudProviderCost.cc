#include "../CloudProviderCost/CloudProviderCost.h"

#include "Management/utils/LogUtils.h"

Define_Module(CloudProviderCost);

CloudProviderCost::~CloudProviderCost(){
}


void CloudProviderCost::initialize(){
    int nIndex;

    EV_INFO << "CloudProviderFirstFit::initialize - Init" << endl;

    // Init super-class
    CloudProviderFirstFit::initialize();
}

void CloudProviderCost::initializeDataCentreCollection(){
    datacentreCollection = new DataCentreInfoCollectionReservation();
}

void CloudProviderCost::initializeRequestHandlers() {
    CloudProviderFirstFit::initializeRequestHandlers();

    requestHandlers[SM_APP_Req_Resume] = [this](SIMCAN_Message *msg) -> void { return handleExtendVmAndResumeExecution(msg); };
    requestHandlers[SM_APP_Req_End] = [this](SIMCAN_Message *msg) -> void { return handleEndVmAndAbortExecution(msg); };
}

void CloudProviderCost::parseConfig() {
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

int CloudProviderCost::parseSlasList (){
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

int CloudProviderCost::parseUsersList (){
    int result;
    const char *userListChr;

    userListChr= par ("userList");
    UserPriorityListParser userParser(userListChr, &vmTypes, &appTypes, &slaTypes);
    result = userParser.parse();
    if (result == SC_OK) {
        userTypes = userParser.getResult();
    }
    //TODO: Cambiar
    userTypes.push_back(new CloudUserPriority("UserTrace", 0, Regular, findSla("Slai0d0c0")));
    return result;
}

int CloudProviderCost::parseDataCentresList (){
    int result;
    const char *dataCentresListChr;

    dataCentresListChr= par ("dataCentresList");
    DataCentreReservationListParser dataCentreParser(dataCentresListChr);
    result = dataCentreParser.parse();
    if (result == SC_OK) {
        dataCentresBase = dataCentreParser.getResult();
    }
    return result;
}

void CloudProviderCost::loadNodes(){
    int nIndex;
    std::vector<int> reservedNodes;
    std::vector<int>::iterator it;

    it = reservedNodes.begin();

    for (nIndex=0; nIndex<dataCentresBase.size(); nIndex++){
        DataCentreReservation* dataCentreReservation = dynamic_cast<DataCentreReservation*>(dataCentresBase.at(nIndex));
        if (dataCentreReservation != nullptr)
        {

            reservedNodes.insert(it+nIndex, dataCentreReservation->getReservedNodes());
        }
    }

    DataCentreInfoCollectionReservation* datacentreCollectionReservation = dynamic_cast<DataCentreInfoCollectionReservation*>(datacentreCollection);

    if (datacentreCollectionReservation != nullptr)
    {
        datacentreCollectionReservation->setReservedNodes(reservedNodes);
    }

    CloudProviderFirstFit::loadNodes();
}

void CloudProviderCost::handleVmRequestFits(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;
    bool bPriorityUser = false;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request"  << endl;

    if(userVM_Rq == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

    CloudUserPriority *pCloudUser = dynamic_cast<CloudUserPriority*>(findUserTypeById(userVM_Rq->getUserID()));

    if(pCloudUser == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong pCloudUser. Null pointer or wrong cloud user class!").c_str());

    userVM_Rq->printUserVM();
    //Check if is a VmRequest or a subscribe
    if (subscribeQueue.size()>0 && pCloudUser->getPriorityType() != Priority)
        rejectVmRequest(userVM_Rq);
    else if (checkVmUserFit(userVM_Rq)) {
        //acceptVmRequest(userVM_Rq);
        EV_FATAL << "OK" << endl;
    } else {
        EV_FATAL << "Fail" << endl;
        rejectVmRequest(userVM_Rq);
    }

}


void CloudProviderCost::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP*>(sm);

    if (userAPP_Rq != nullptr) {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage (sm, toDataCentreGates[0]);
    }
    else
    {
      error ("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }
}

void CloudProviderCost::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP*>(sm);

    if (userAPP_Rq != nullptr) {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage (sm, toDataCentreGates[0]);
    }
    else
    {
      error ("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }

}

std::string CloudProviderCost::slasToString (){

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

Sla* CloudProviderCost::findSla (std::string slaType){

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

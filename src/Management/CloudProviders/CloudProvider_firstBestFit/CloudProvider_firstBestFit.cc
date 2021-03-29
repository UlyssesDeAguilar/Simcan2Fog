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

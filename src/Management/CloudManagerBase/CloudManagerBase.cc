#include "CloudManagerBase.h"

CloudManagerBase::~CloudManagerBase(){
    vmTypes.clear();
    userTypes.clear();
    appTypes.clear();

    selfHandlers.clear();
    requestHandlers.clear();
    responseHandlers.clear();
}


void CloudManagerBase::initialize(){
    // Init super-class
    cSIMCAN_Core::initialize();

    bFinished = false;

    parseConfig();

    initializeSelfHandlers();
    initializeRequestHandlers();
    initializeResponseHandlers();

}

void CloudManagerBase::parseConfig() {
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

int CloudManagerBase::parseAppList (){
    int result;
    const char *appListChr;

    appListChr= par ("appList");
    AppListParser appParser(appListChr);
    result = appParser.parse();
    if (result == SC_OK) {
        appTypes = appParser.getResult();
    }
    return result;
}

int CloudManagerBase::parseVmsList (){
    int result;
    const char *vmListChr;

    vmListChr= par ("vmList");
    VmListParser vmParser(vmListChr);
    result = vmParser.parse();
    if (result == SC_OK) {
        vmTypes = vmParser.getResult();
    }
    return result;
}

int CloudManagerBase::parseUsersList (){
    int result;
    const char *userListChr;

    userListChr= par ("userList");
    UserListParser userParser(userListChr, &vmTypes, &appTypes);
    result = userParser.parse();
    if (result == SC_OK) {
        userTypes = userParser.getResult();
    }
    return result;

//    int result;
//    const char *userListChr;
//
//    userListChr= par ("userList");
//    UserListParser userParser(userListChr, &vmTypes, &appTypes);
//    result = userParser.parse();
//    if (result == SC_OK) {
//        userTypes = userParser.getResult();
//    }
//    return result;
}


void CloudManagerBase::processSelfMessage (cMessage *msg){
    std::map<std::string, std::function<void(cMessage*)>>::iterator it;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << endl;

    it = selfHandlers.find(msg->getName());

    if (it != selfHandlers.end())
        it->second(msg);  //Perform the operations...
    else
        error ("Unknown self message [%s]", msg->getName());

    cancelAndDelete(msg);

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;
}

void CloudManagerBase::processRequestMessage (SIMCAN_Message *sm)
{
    SM_CloudProvider_Control* userControl;
    std::map<int, std::function<void(SIMCAN_Message*)>>::iterator it;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << endl;

    userControl = dynamic_cast<SM_CloudProvider_Control *>(sm);

    if(userControl != nullptr)
      {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received end of party"  << endl;
        //Stop the checking process.
        bFinished = true;
        cancelAndDelete(userControl);
      } else {
          it = requestHandlers.find(sm->getOperation());

          if (it != requestHandlers.end())
            {
              it->second(sm);
            }
      }
}

void CloudManagerBase::processResponseMessage (SIMCAN_Message *sm){
    SM_CloudProvider_Control* userControl;
    std::map<int, std::function<void(SIMCAN_Message*)>>::iterator it;
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << endl;

    it = responseHandlers.find(sm->getResult());

    if (it != responseHandlers.end())
      {
        //Perform the operations...
        it->second(sm);
      }
}




std::string CloudManagerBase::appsToString (){

    std::ostringstream info;
    int currentApp, currentParameter;

        // Main text for the applications of this manager
        info << std::endl << appTypes.size() << " applications parsed from ManagerBase in " << getFullPath() << endl << endl;

            // For each application
            for (currentApp=0; currentApp<appTypes.size(); currentApp++){

                info << "\tApplication[" << currentApp << "] --> " << appTypes.at(currentApp)->getAppName()  << ":" << appTypes.at(currentApp)->getType()
                     << " (" << appTypes.at(currentApp)->getNumParameters() << " parameters)" << endl;

                // For each parameter in the current application
                for (currentParameter = 0; currentParameter < appTypes.at(currentApp)->getNumParameters(); currentParameter++){
                    info << "\t   + " << appTypes.at(currentApp)->getParameter(currentParameter)->toString() << endl;
                }

                info << endl;
            }

    info << "---------------- End of parsed Applications in " << getFullPath() << " ----------------" << endl;

    return info.str();
}


std::string CloudManagerBase::vmsToString (){

    std::ostringstream info;
    int i;

        info.str("");

        // Main text for the VMs of this manager
        info << std::endl << vmTypes.size() << " VMs parsed from ManagerBase in " << getFullPath() << endl << endl;

        // For each VM...
        for (i=0; i<vmTypes.size(); i++){
            info << "\tVM[" << i << "]  --> " << vmTypes.at(i)->getType() << " : " << vmTypes.at(i)->featuresToString() << endl;
        }

        info << endl;
        info << "---------------- End of parsed VMs in " << getFullPath() << " ----------------" << endl;

    return info.str();
}

std::string CloudManagerBase::usersToString (){

    std::ostringstream info;
    int i;

        // Main text for the users of this manager
        info << std::endl << userTypes.size() << " CloudUsers parsed from ManagerBase in " << getFullPath() << endl << endl;

        for (i=0; i<userTypes.size(); i++){
            info << "\tUser[" << i << "]  --> " << userTypes.at(i)->toString() << endl;
        }

        info << "---------------- End of parsed CloudUsers in " << getFullPath() << " ----------------" << endl;

    return info.str();
}


Application* CloudManagerBase::findApplication (std::string appName){

    std::vector<Application*>::iterator it;
    Application* result;
    bool found;

        // Init
        found = false;
        result = nullptr;
        it = appTypes.begin();

        // Search...
        while((!found) && (it != appTypes.end())){

            if ((*it)->getAppName() == appName){
                found = true;
                result = *it;
            }
            else
                it++;
        }

    return result;
}


VirtualMachine* CloudManagerBase::findVirtualMachine (std::string vmType){

    std::vector<VirtualMachine*>::iterator it;
    VirtualMachine* result;
    bool found;

        // Init
        found = false;
        result = nullptr;
        it = vmTypes.begin();

        // Search...
        while((!found) && (it != vmTypes.end())){

            if ((*it)->getType() == vmType){
                found = true;
                result = (*it);
            }
            else
                it++;
        }

    return result;
}

CloudUser* CloudManagerBase::findUser (std::string userType){

    std::vector<CloudUser*>::iterator it;
    CloudUser* result;
    bool found;

        // Init
        found = false;
        result = nullptr;
        it = userTypes.begin();

        // Search...
        while((!found) && (it != userTypes.end())){

            if ((*it)->getType() == userType){
                found = true;
                result = (*it);
            }
            else
                it++;
        }

    return result;
}

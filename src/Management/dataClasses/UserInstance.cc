#include "UserInstance.h"

UserInstance::UserInstance(User *ptrUser, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances){

    std::ostringstream osStream;
    UserAppReference* appReference;
    int currentApp;

    // Init main attributes
    this->type = ptrUser->getType();
    this->userNumber = userNumber;
    this->instanceNumber = currentInstanceIndex;

    // Generate userID
    osStream << "(" << userNumber << ")" << this->type << "[" << (currentInstanceIndex+1) << "/" << totalUserInstances << "]";
    userID = osStream.str();

    // Include app instances
    for (currentApp = 0; currentApp < ptrUser->getNumApplications(); currentApp++){

        // Get current application
        appReference = ptrUser->getApplication(currentApp);

        // Insert a new collection of application instances
        insertNewApplicationInstances (appReference->getAppBase(), appReference->getNumInstances());
    }
}


UserInstance::~UserInstance() {
    applications.clear();
}


unsigned int UserInstance::getInstanceNumber() const {
    return instanceNumber;
}


const std::string& UserInstance::getType() const {
    return type;
}


const std::string& UserInstance::getUserID() const {
    return userID;
}


unsigned int UserInstance::getUserNumber() const {
    return userNumber;
}


void UserInstance::insertNewApplicationInstances (Application* appPtr, int numInstances){

    AppInstanceCollection* newAppCollection;

    newAppCollection = new AppInstanceCollection (appPtr, this->userID, numInstances);
    applications.push_back(newAppCollection);
}

std::string UserInstance::toString (bool includeAppsParameters){

    std::ostringstream info;
    int i;

        info << "Name:" << userID << std::endl;

            // Parses applications
            for (i=0; i<applications.size(); i++){
                info << "\tAppCollection[" << i << "] -> " << applications.at(i)->toString(includeAppsParameters);
            }

    return info.str();
}

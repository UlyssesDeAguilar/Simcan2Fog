#include "AppInstance.h"

AppInstance::AppInstance(std::string appName, int currentInstanceIndex, int totalAppInstances, std::string userID){

    std::ostringstream osStream;

        // Generate the unique name -> instanceName:instanceType(index/total)[userName]
        osStream << appName << "-" << userID << "-[" << currentInstanceIndex+1 << "/" << totalAppInstances << "]";
        this->appInstanceID = osStream.str();

        // Set the main parameters
        this->appName = appName;
        this->instanceNumber = currentInstanceIndex;
        this->userID = userID;

        // App state
        this->state = appWaiting;
}

AppInstance::~AppInstance() {
}

const std::string& AppInstance::getAppInstanceId() const {
    return appInstanceID;
}

const std::string& AppInstance::getVmInstanceId() const {
    return vmInstanceID;
}

void AppInstance::setVmInstanceId(std::string vmInstanceID) {
    this->vmInstanceID = vmInstanceID;
}

const std::string& AppInstance::getAppName() const {
    return appName;
}

int AppInstance::getInstanceNumber() const {
    return instanceNumber;
}

tApplicationState AppInstance::getState() const {
    return state;
}

void AppInstance::setState(tApplicationState state) {
    this->state = state;
}

const std::string& AppInstance::getUserId() const {
    return userID;
}

std::string AppInstance::stateToString (tApplicationState appState){

    std::string result;

        switch (appState){

                    case appWaiting:
                        result = "waiting";
                        break;
                    case appRunning:
                        result = "running";
                        break;
                    case appFinishedOK:
                        result = "finishedOK";
                        break;
                    case appFinishedTimeout:
                        result = "finishedTimeout";
                        break;
                    case appFinishedError:
                        result = "finishedError";
                        break;
                    default:
                        result = "unkown state";
                        break;
        }

    return result;
}

std::string AppInstance::toString (){

    std::ostringstream info;

        info << appInstanceID << "  - State: " << stateToString(state);

    return info.str();
}

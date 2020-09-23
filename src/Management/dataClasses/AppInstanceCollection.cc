#include "AppInstanceCollection.h"

AppInstanceCollection::AppInstanceCollection(Application* appPtr, std::string userID, int numInstances){

    // Check!
    if (appPtr == nullptr)
        throw omnetpp::cRuntimeError("Constructor of AppInstanceCollection receives a null pointer to the base Application. #Instances:%d - User:%s", numInstances, userID.c_str());

    // AppInstance pointer to maintain general information of the application
    appBase = appPtr;

    // Generate the replicas
    this->generateInstances(userID, numInstances);
}

AppInstanceCollection::~AppInstanceCollection() {
    appInstances.clear();
}

void AppInstanceCollection::generateInstances (std::string userID, int numInstances){

    AppInstance *newInstance;
    int i;

        // Create and include replicas in the replica vector
        for (i=0; i<numInstances; i++){
            newInstance = new AppInstance(appBase->getAppName(), i, numInstances, userID);
            appInstances.push_back(newInstance);
        }
}

Application* AppInstanceCollection::getApplicationBase (){
    return appBase;
}

int AppInstanceCollection::getNumInstances (){
    return appInstances.size();
}
AppInstance* AppInstanceCollection::getInstance(int nInstance)
{
    AppInstance* pInstance;

    pInstance = nullptr;

    if(nInstance < appInstances.size())
        pInstance = appInstances.at(nInstance);


    return pInstance;
}
std::string AppInstanceCollection::toString (bool showParameters){

    std::ostringstream info;
    int i, currentParameter;

        info << "# Instances:" << appInstances.size() << " of " << appBase->getAppName() << "(" << appBase->getType() << ")" << std::endl;

        for (i=0; i<appInstances.size(); i++){
            info << "\t\t  - appInstance[" << i << "]: " << appInstances.at(i)->toString() << std::endl;
        }

        if (showParameters){

            info << std::endl;
            info << "\t\t\tParameters:" << std::endl;

            for (currentParameter = 0; currentParameter < appBase->getNumParameters(); currentParameter++){
                info << "\t\t\t  > " << appBase->getParameter(currentParameter)->toString() << std::endl;
            }
        }

        info << std::endl;


    return info.str();
}

#include "../../../management/dataClasses/Applications/AppInstanceCollection.h"

AppInstanceCollection::AppInstanceCollection(const Application* appPtr, std::string userID, int numInstances){

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
            newInstance = new AppInstance(appBase->getName(), i, numInstances, userID);
            appInstances.push_back(newInstance);
        }
}

const Application* AppInstanceCollection::getApplicationBase (){
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
    int i;

        info << "# Instances:" << appInstances.size() << " of " << appBase->getName() << "(" << appBase->getType() << ")" << "\n";

        for (i=0; i<appInstances.size(); i++){
            info << "\t\t  - appInstance[" << i << "]: " << appInstances.at(i)->toString() << "\n";
        }

        if (showParameters){

            info << "\n";
            info << "\t\t\tParameters:" << "\n";

            for (const auto &iter: appBase->getAllParameters())
            {
                info << "\t\t\t  > " << iter.first << " = " << iter.second << "\n";
            }
        }

        info << "\n";


    return info.str();
}

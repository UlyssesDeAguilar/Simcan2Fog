#include "../../../management/dataClasses/Users/UserInstance.h"

UserInstance::UserInstance(const User *user, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances)
{

    // Init main attributes
    if (user != nullptr)
    {
        std::ostringstream osStream;
        this->type = user->getType();
        this->userNumber = userNumber;
        this->instanceNumber = currentInstanceIndex;

        // Generate userID
        osStream << "(" << userNumber << ")" << this->type << "[" << (currentInstanceIndex + 1) << "/" << totalUserInstances << "]";
        id = osStream.str();

        // Include and generete the app instances
        for (const auto &app : user->allApplications())
            insertNewApplicationInstances(app->getAppBase(), app->getNumInstances());
    }
    else
    {
        // Warning! it is only used on traced files
    }
}

UserInstance::~UserInstance()
{
    applications.clear();
}

void UserInstance::insertNewApplicationInstances(const Application *appPtr, int numInstances)
{
    auto newAppCollection = new AppInstanceCollection(appPtr, this->id, numInstances);
    applications.push_back(newAppCollection);
}

std::string UserInstance::toString(bool includeAppsParameters)
{

    std::ostringstream info;
    int i;

    info << "Name:" << id << std::endl;

    // Parses applications
    for (i = 0; i < applications.size(); i++)
    {
        info << "\tAppCollection[" << i << "] -> " << applications.at(i)->toString(includeAppsParameters);
    }

    return info.str();
}

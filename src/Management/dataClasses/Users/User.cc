#include "User.h"

User::User(std::string type, int numInstances)
{
    this->type = type;
    this->numInstances = numInstances;
}

User::~User()
{
    applications.clear();
}

void User::insertApplication(const Application *appPtr, int numInstances)
{
    auto newElement = new UserAppReference(appPtr, numInstances);
    applications.push_back(newElement);
}

std::string User::toString() const
{
    std::ostringstream info;

    info << "Type:" << type << "  -  #Instances:" << numInstances << std::endl;
    
    // For all applications
    int i = 0;
    for (auto const &app: applications)
        info << "\tApp[" << i << "] -> " << app->toString();

    return info.str();
}

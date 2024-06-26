#include "../../../management/dataClasses/Users/UserAppReference.h"

std::string UserAppReference::toString()
{
    std::ostringstream info;
    info << appBase->getName() << ":" << appBase->getType() << " - " << numInstances << " instances";
    return info.str();
}
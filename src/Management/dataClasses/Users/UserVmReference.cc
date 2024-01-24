#include "UserVmReference.h"

std::string UserVmReference::toString()
{
    std::ostringstream info;
    info << vmBase->getType() << " - " << numInstances << " instances "
         << " - " << nRentTime << " renting hours";
    return info.str();
}

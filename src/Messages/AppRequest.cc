#include "AppRequest.h"

std::ostream &operator<<(std::ostream &os, const APP_Request &obj)
{
    os << "App: " << obj.strApp
       << " | status: " << obj.eState
       << " | Ip:" << obj.strIp
       << " | VmId: " << obj.vmId
       << " | startTime: " << obj.startTime
       << " | endTime: " << obj.finishTime;
    return os;
}
#include "../messages/AppRequest.h"

std::ostream &operator<<(std::ostream &os, const AppRequest &obj)
{
    os << "App: " << obj.serviceName
       << " | status: " << obj.state
       << " | startTime: " << obj.startTime
       << " | endTime: " << obj.finishTime;
    return os;
}
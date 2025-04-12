#include "../messages/SM_UserAPP.h"
#include "../management/utils/LogUtils.h"

Register_Class(SM_UserAPP);

/*void SM_UserAPP::update(const SM_UserAPP *newData)
{
    int newFinishedApps = 0;
    for (const auto &request : newData->apps)
    {
        // Linear search
        for (auto &original : apps)
        {
            // Match the request <- TODO: Maybe implement and == operator for AppRequest
            if (original.serviceName == request.serviceName)
            {
                original.state = request.state;

                // If it went from Waiting/Running to Finished
                if (!AppRequest::isFinished(original) && AppRequest::isFinished(request))
                    newFinishedApps++;
            }
        }
    }

    // Update the finished apps
    nFinishedApps += newFinishedApps;
}*/

void SM_UserAPP::createNewAppRequest(const char *name, const char *type, double startRentTime)
{
    AppRequest appRQ;
    appRQ.setName(name);
    appRQ.setType(type);
    appRQ.setState(appWaiting);
    appRQ.setStartTime(startRentTime);
    appendApp(appRQ);
}

int SM_UserAPP::findRequestIndex(const char *name)
{
    for (int i = 0; i < app_arraysize; i++)
    {
        if (strcmp(app[i].getName(), name) == 0)
            return i;
    }

    return -1;
}

void SM_UserAPP::changeState(const char *service, tApplicationState eNewState) { changeStateByIndex(findRequestIndex(service), eNewState); }
void SM_UserAPP::changeStateByIndex(int nIndex, tApplicationState eNewState) { app[nIndex].setState(eNewState); }

bool SM_UserAPP::isFinishedOK(const char *service) { return app[findRequestIndex(service)].isFinishedOK(); }
bool SM_UserAPP::isFinishedKO(const char *service) { return app[findRequestIndex(service)].isFinishedKO(); }
bool SM_UserAPP::allAppsFinishedOK()
{
    for (int i = 0; i < app_arraysize; i++)
    {
        if (!app[i].isFinishedOK())
            return false;
    }

    return true;
}

bool SM_UserAPP::allAppsFinished()
{
    for (int i = 0; i < app_arraysize; i++)
    {
        if (!app[i].isFinished())
            return false;
    }

    return true;
}

void SM_UserAPP::abortAllApps()
{
    for (int i = 0; i < app_arraysize; i++)
    {
        if (!app[i].isFinished())
        {
            app[i].setState(appFinishedTimeout);

            EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                    << " - Application[" << i << "] " << app[i].getName()
                    << " in VM " << vmId
                    << " has been aborted by timeout" << '\n';
        }
        i++;
    }
}

std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj)
{
    // Print Message
    os << "User received: " << obj.userId << '\n';
    os << "Associated VM: " << obj.vmId << '\n';
    // EV_INFO << "Finished: " << this->getNFinishedApps() <<'\n';
    // EV_INFO << "All: " << this->allAppsFinished() <<'\n';
    // EV_INFO << "Ok: " << this->allAppsFinishedOK() <<'\n';
    // EV_INFO << "Trace: " << this->trace.empty() <<'\n';
    os << "Total requests received: " << obj.getAppArraySize() << '\n';

    for (int i = 0; i < obj.getAppArraySize(); i++)
        os << "+RQ(" << i + 1 << "): " << obj.getApp(i).str() << '\n';

    return os;
}

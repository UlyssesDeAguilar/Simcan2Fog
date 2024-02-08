#include "SM_UserAPP.h"
#include "Management/utils/LogUtils.h"

Register_Class(SM_UserAPP);

SM_UserAPP::SM_UserAPP() : SM_UserAPP_Base("SM_UserAPP", 0)
{
    vmId = nullptr;
    userID = nullptr;
    nFinishedApps = 0;
}

void SM_UserAPP::setApp(size_t k, const APP_Request &app)
{
    if (k > apps.size())
        throw cRuntimeError(
            "Index overflow at %u when last app id index was %u", k, apps.size());

    apps[k] = app;
}

SM_UserAPP::~SM_UserAPP()
{
    // FIXME Consider maybe the message included in APP_Request ?
}

SM_UserAPP::SM_UserAPP(const SM_UserAPP &other) : SM_UserAPP_Base(other)
{
    copy(other);
}

SM_UserAPP &SM_UserAPP::operator=(const SM_UserAPP &other)
{
    if (this == &other)
        return *this;
    SM_UserAPP_Base::operator=(other);
    copy(other);
    return *this;
}

void SM_UserAPP::copy(const SM_UserAPP &other)
{
    // Copy parameters
    // apps = other.apps;
    vmAppGroupedVector = other.vmAppGroupedVector;
}

SM_UserAPP *SM_UserAPP::dup(const std::string &vmId) const
{
    int nFinished = 0; // Finished app requests (OK or not)
    SM_UserAPP *pRet = new SM_UserAPP();

    // Copy all context from the base (we will change the nFinishedApps)
    pRet->SM_UserAPP_Base::operator=(*this);

    auto iter = std::find(vmAppGroupedVector.begin(), vmAppGroupedVector.end(), vmId);

    if (iter != vmAppGroupedVector.end())
    {
        for (const auto &appReq : *iter)
        {
            pRet->vmAppGroupedVector.emplace_back(appReq);
            if (APP_Request::isFinished(appReq))
                nFinished++;
        }
        pRet->vmAppGroupedVector.at(0) = vmId;
    }

    // Set the finished apps
    pRet->nFinishedApps = nFinished;

    // Return the copy
    return pRet;
}

void SM_UserAPP::update(const SM_UserAPP *newData)
{
    int newFinishedApps = 0;
    for (const auto &request : newData->apps)
    {
        // Linear search
        for (auto &original : apps)
        {
            // Match the request <- TODO: Maybe implement and == operator for APP_Request
            if (original.vmId == request.vmId && original.strApp == request.strApp)
            {
                original.eState = request.eState;

                // If it went from Waiting/Running to Finished
                if (!APP_Request::isFinished(original) && APP_Request::isFinished(request))
                    newFinishedApps++;
            }
        }
    }

    // Update the finished apps
    nFinishedApps += newFinishedApps;
}

int SM_UserAPP::findRequestIndex(const std::string &service, const std::string &vmId)
{
    auto selector = [service, vmId](APP_Request appRq) -> bool
    {
        return appRq.strApp == service && vmId == appRq.vmId;
    };

    auto iter = std::find_if(apps.begin(), apps.end(), selector);

    if (iter == apps.end())
    {
        EV_INFO << "SM_UserAPP::findRequestIndex::Application request not found, app: "
                << service << " | vmId: " << vmId << '\n';
        return -1;
    }

    // This is only valid because apps is a vector!
    return iter - apps.begin();
}

void SM_UserAPP::changeState(const std::string &service, const std::string &vmId, tApplicationState eNewState)
{
    int nIndex = findRequestIndex(service, vmId);
    changeStateByIndex(nIndex, eNewState);
}

void SM_UserAPP::changeStateByIndex(int nIndex, tApplicationState eNewState)
{
    auto request = getApp(nIndex);

    // If it finished -> record finish time
    if (!APP_Request::isFinished(request) && eNewState == appFinishedOK)
        request.finishTime = simTime().dbl();

    // Update
    request.eState = eNewState;
}

bool SM_UserAPP::allAppsFinishedOK(const std::string &vmId)
{
    auto predicate = [vmId](const APP_Request &appRq) -> bool
    {
        return appRq.vmId == vmId && appRq.eState == appFinishedOK;
    };

    return std::all_of(apps.begin(), apps.end(), predicate);
}

bool SM_UserAPP::isFinishedOK(const std::string &service, const std::string &vmId)
{
    int nIndex = findRequestIndex(service, vmId);
    return nIndex != -1 && APP_Request::isFinishedOK(apps[nIndex]);
}

bool SM_UserAPP::isFinishedKO(const std::string &service, const std::string &vmId)
{
    int nIndex = findRequestIndex(service, vmId);
    return nIndex != -1 && APP_Request::isFinishedKO(apps[nIndex]);
}

bool SM_UserAPP::allAppsFinished(const std::string &vmId)
{
    for (const auto &request : apps)
    {
        if (request.vmId == vmId && !APP_Request::isFinished(request))
            return false;
    }

    return true;
}

void SM_UserAPP::abortAllApps(const std::string &vmId)
{
    // Selection criteria
    // 1 - Is associated with the vm
    // 2 - It didn't finish execution
    auto selected = [vmId](const APP_Request &appRq) -> bool
    {
        return appRq.vmId == vmId &&
               !(APP_Request::isFinished(appRq));
    };

    int i = 0;
    for (auto &appRq : apps)
    {
        if (selected(appRq))
        {
            appRq.eState = appFinishedTimeout;
            appRq.finishTime = simTime().dbl();

            EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                    << " - Application[" << i << "] " << appRq.strApp
                    << " in VM " << vmId
                    << " has been aborted by timeout" << '\n';
            increaseFinishedApps();
        }
        i++;
    }
}

std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj)
{
    // Print Message
    os << "User received: " << obj.userID << '\n';
    os << "Associated VM: " << obj.vmId << '\n';
    // EV_INFO << "Finished: " << this->getNFinishedApps() <<'\n';
    // EV_INFO << "All: " << this->allAppsFinished() <<'\n';
    // EV_INFO << "Ok: " << this->allAppsFinishedOK() <<'\n';
    // EV_INFO << "Trace: " << this->trace.empty() <<'\n';
    os << "Total requests received: " << obj.getAppArraySize() << '\n';

    int i = 0;
    for (const auto &request : obj.apps)
        os << "+RQ(" << i++ << "): " << request << '\n';

    return os;
}

#include "SM_UserAPP.h"
#include "Management/utils/LogUtils.h"

Register_Class(SM_UserAPP);

SM_UserAPP::SM_UserAPP()
{
    vmId = nullptr;
    userID = nullptr;
    app = nullptr;
    finished = false;
    nFinishedApps = 0;
    idCounter = 0;
    appArraySize = 0;
}

void SM_UserAPP::setAppArraySize(size_t size)
{
    // Check the new size is actually bigger
    // If not, don't waste cpu cycles copying around
    if (size < appArraySize)
        return;

    // Create the new array
    APP_Request *app2 = new APP_Request[size];

    // Copy the the contents until it there is no more relevant info
    for (int i = 0; i < idCounter; i++)
        app2[i] = app[i];
    
    delete[] app;

    // Swap and set new size
    app = app2;
    appArraySize = size;
}

APP_Request &SM_UserAPP::getApp(size_t k)
{
    if (k > idCounter)
        throw cRuntimeError(
            "Index overflow at %u when last app id index was %u", k, idCounter);

    return app[k];
}

void SM_UserAPP::setApp(size_t k, const APP_Request &app)
{
    if (k > idCounter)
        throw cRuntimeError(
            "Index overflow at %u when last app id index was %u", k, idCounter);

    this->app[k] = app;
}

SM_UserAPP::~SM_UserAPP()
{
    // FIXME Consider maybe the message included in APP_Request ?
    delete[] app;
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
    // Copy parameters and prepare the array
    appArraySize = other.appArraySize;
    idCounter = other.idCounter;
    app = new APP_Request[appArraySize];

    // Copy the real instances
    for (int i = 0; i < idCounter; i++)
        copyAppRequest(other.app[i], app[i]);
}

void SM_UserAPP::copyAppRequest(const APP_Request &src, APP_Request &dest)
{
    dest.startTime = src.startTime;
    dest.finishTime = src.finishTime;
    dest.strIp = src.strIp;
    dest.strApp = src.strApp;
    dest.strAppType = src.strAppType;
    dest.eState = src.eState;
    dest.vmId = src.vmId;

    if (src.pMsgTimeout != nullptr)
        dest.pMsgTimeout = src.pMsgTimeout->dup();
    else
        dest.pMsgTimeout = nullptr;
}

/*int SM_UserAPP::createNewAppRequest(std::string strService,std::string strIp,std::string strVmId,int nStartRentTime)
{
    APP_Request appRQ;

    appRQ.startTime=nStartRentTime;
    appRQ.strIp=strIp;
    appRQ.strApp=strService;
    appRQ.eState = appWaiting;
    //appRQ.vmId=strVmId;

   // EV_INFO << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
   //         " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}*/
int SM_UserAPP::createNewAppRequestFull(std::string strApp, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish *pMsgTimeout)
{
    APP_Request appRQ;

    appRQ.startTime = startTime;
    appRQ.strIp = strIp;
    appRQ.strApp = strApp;
    appRQ.strAppType = "";
    appRQ.eState = eState;
    appRQ.vmId = vmId;

    if (pMsgTimeout != nullptr)
    {
        appRQ.pMsgTimeout = new SM_UserAPP_Finish();
        appRQ.pMsgTimeout->setUserID(pMsgTimeout->getUserID());
        appRQ.pMsgTimeout->setStrVmId(pMsgTimeout->getStrVmId());
        appRQ.pMsgTimeout->setStrApp(pMsgTimeout->getStrApp());
        appRQ.pMsgTimeout->setNTotalTime(pMsgTimeout->getNTotalTime());
    }
    else
        appRQ.pMsgTimeout = nullptr;
    // appRQ.vmId=strVmId;

    // EV_INFO << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
    //         " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequestFull(std::string strApp, std::string strAppType, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish *pMsgTimeout)
{
    APP_Request appRQ;

    appRQ.startTime = startTime;
    appRQ.finishTime = finishTime;
    appRQ.strIp = strIp;
    appRQ.strApp = strApp;
    appRQ.strAppType = strAppType;
    appRQ.eState = eState;
    appRQ.vmId = vmId;

    if (pMsgTimeout != nullptr)
    {
        appRQ.pMsgTimeout = new SM_UserAPP_Finish();
        appRQ.pMsgTimeout->setUserID(pMsgTimeout->getUserID());
        appRQ.pMsgTimeout->setStrVmId(pMsgTimeout->getStrVmId());
        appRQ.pMsgTimeout->setStrApp(pMsgTimeout->getStrApp());
        appRQ.pMsgTimeout->setNTotalTime(pMsgTimeout->getNTotalTime());
    }
    else
        appRQ.pMsgTimeout = nullptr;
    // appRQ.vmId=strVmId;

    // EV_INFO << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
    //         " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequest(std::string strService, std::string strIp, std::string strVmId, int nStartRentTime)
{
    APP_Request appRQ;

    appRQ.startTime = nStartRentTime;
    appRQ.finishTime = 0;
    appRQ.strIp = strIp;
    appRQ.strApp = strService;
    appRQ.strAppType = strService;
    appRQ.eState = appWaiting;
    appRQ.pMsgTimeout = nullptr;
    appRQ.vmId = strVmId;

    EV_DEBUG << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:" << appRQ.strIp << " | VmId: " << appRQ.vmId << " | startTime: " << appRQ.startTime << " | endTime: " << appRQ.finishTime << endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequest(std::string strService, std::string strAppType, std::string strIp, std::string strVmId, int nStartRentTime)
{
    APP_Request appRQ;

    appRQ.startTime = nStartRentTime;
    appRQ.finishTime = 0;
    appRQ.strIp = strIp;
    appRQ.strApp = strService;
    appRQ.strAppType = strAppType;
    appRQ.eState = appWaiting;
    appRQ.pMsgTimeout = nullptr;
    appRQ.vmId = strVmId;

    EV_DEBUG << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:" << appRQ.strIp << " | VmId: " << appRQ.vmId << " | startTime: " << appRQ.startTime << " | endTime: " << appRQ.finishTime << endl;

    return addAppRequest(appRQ);
}

SM_UserAPP *SM_UserAPP::dup(std::string strVmId) const
{
    int nFinished = 0; // Finished app requests (OK or not)
    SM_UserAPP *pRet = new SM_UserAPP();

    // Copy all context from the base (we will change the nFinishedApps)
    pRet->SM_UserAPP_Base::operator=(*this);

    // Calculate and set the finished apps
    for (int i = 0; i < getAppArraySize(); i++)
    {
        APP_Request appReq = this->getApp(i);
        if (strVmId.compare(appReq.vmId) == 0)
        {
            pRet->createNewAppRequestFull(appReq.strApp, appReq.strAppType, appReq.strIp, appReq.vmId, appReq.startTime, appReq.finishTime, appReq.eState, appReq.pMsgTimeout);
            if (appReq.eState == appFinishedOK || appReq.eState == appFinishedTimeout || appReq.eState == appFinishedError)
                nFinished++;
        }
    }
    pRet->nFinishedApps = nFinished;

    // Return the copy
    return pRet;
}

void SM_UserAPP::update(SM_UserAPP *newData)
{
    std::string strVmId = newData->getVmId();
    int totalFinished = 0;
    for (int i = 0; i < getAppArraySize(); i++)
    {
        APP_Request appReq = getApp(i);
        if (strVmId.compare(appReq.vmId) == 0)
        {
            for (int j = 0; j < newData->getAppArraySize(); j++)
            {
                APP_Request appReq2 = newData->getApp(j);
                if (appReq.strApp.compare(appReq2.strApp) == 0)
                {
                    changeStateByIndex(i, appReq.strApp, appReq2.eState);
                    if (appReq2.pMsgTimeout != nullptr)
                        getApp(i).pMsgTimeout = appReq2.pMsgTimeout;
                    break;
                }
            }
        }
        appReq = getApp(i);
        if (appReq.eState == appFinishedOK || appReq.eState == appFinishedTimeout || appReq.eState == appFinishedError)
            totalFinished++;
    }

    // Reserve memory to trace!
    // setTraceArraySize (getTraceArraySize() + newData->getTraceArraySize());

    // Copy trace!
    // for (int i = 0; i < newData->trace.size(); i++)
    //    addNodeTrace(newData->trace[i].first, newData->trace[i].second);

    setNFinishedApps(totalFinished);
}

int SM_UserAPP::findRequestIndex(std::string strService, std::string strVmId)
{
    APP_Request appRq;
    int nIndex, nRet;
    bool bFound;

    bFound = false;
    nIndex = 0;

    while (!bFound && nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        if (appRq.strApp.compare(strService) == 0 && strVmId.compare(appRq.vmId) == 0)
        {
            nRet = nIndex;
            bFound = true;
        }
        nIndex++;
    }
    if (!bFound)
    {
        EV_INFO << "SM_UserAPP::findRequestIndex::Application request not found, app: " << strService << " | vmId: " << strVmId << endl;
    }
    return nRet;
}
void SM_UserAPP::changeState(std::string strService, std::string strVmId, tApplicationState eNewState)
{
    APP_Request *pAppRq;
    int nIndex;

    nIndex = findRequestIndex(strService, strVmId);

    if (nIndex < getAppArraySize())
    {
        getApp(nIndex).eState = eNewState;
    }
    else
    {
        EV_INFO << "SM_UserAPP::changeState::Application request not found!!!" << endl;
        printUserAPP();
    }
}
void SM_UserAPP::setVmIdByIndex(int nIndex, std::string strIp, std::string strVmId)
{
    if (nIndex < getAppArraySize())
    {
        // Use the ip to compare
        if (strIp.compare(getApp(nIndex).strIp) == 0)
        {
            getApp(nIndex).vmId = strVmId;
        }
    }
    else
    {
        EV_INFO << "SM_UserAPP::changeState::Application request not found!!!" << endl;
        printUserAPP();
    }
}
void SM_UserAPP::changeStateByIndex(int nIndex, std::string strService, tApplicationState eNewState)
{
    APP_Request *pAppRq;

    if (nIndex < getAppArraySize())
    {
        if (strService.compare(getApp(nIndex).strApp) == 0)
        {
            getApp(nIndex).eState = eNewState;
        }
    }
    else
    {
        EV_INFO << "SM_UserAPP::changeState::Application request not found!!!" << endl;
        printUserAPP();
    }
}
void SM_UserAPP::setStartTime(std::string strService, std::string strIp, double dTime)
{
    int nIndex;

    nIndex = findRequestIndex(strService, strIp);

    if (nIndex < getAppArraySize())
    {
        getApp(nIndex).startTime = dTime;
    }
    else
    {
        EV_INFO << "SM_UserAPP::setStartTime::Application request not found!!!" << endl;
        printUserAPP();
    }
}
bool SM_UserAPP::isFinishedOK(std::string strService, std::string strIp)
{
    bool bRet;
    int nIndex;

    bRet = false;
    nIndex = findRequestIndex(strService, strIp);

    if (nIndex != -1 && nIndex < getAppArraySize())
    {
        bRet = (getApp(nIndex).eState == appFinishedOK);
    }
    else
    {
        EV_INFO << "SM_UserAPP::changeState::Application request not found!!!" << endl;
        printUserAPP();
    }
    return bRet;
}

bool SM_UserAPP::allAppsFinishedOK(std::string strVmId)
{
    bool bRet;

    APP_Request appRq;
    int nIndex;
    bool bFinished;

    bFinished = true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - Init" << endl;
    while (bFinished && nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        if (appRq.vmId.compare(strVmId) == 0)
            bFinished = (appRq.eState == appFinishedOK);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - App " << appRq.strApp << " | status " << stateToString(appRq.eState) << endl;
        nIndex++;
    }

    bRet = bFinished;
    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - End" << endl;

    return bRet;
}

bool SM_UserAPP::allAppsFinishedOK()
{
    bool bRet;

    APP_Request appRq;
    int nIndex;
    bool bFinished;

    bFinished = true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - Init" << endl;
    while (bFinished && nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        bFinished = (appRq.eState == appFinishedOK);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - App " << appRq.strApp << " | status " << stateToString(appRq.eState) << endl;
        nIndex++;
    }

    bRet = bFinished;
    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - End" << endl;

    return bRet;
}

bool SM_UserAPP::allAppsFinishedKO()
{
    bool bRet;

    APP_Request appRq;
    int nIndex;
    bool bFinished;

    bFinished = true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - Init" << endl;
    while (bFinished && nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        bFinished = (appRq.eState == appFinishedError || appRq.eState == appFinishedTimeout);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - App " << appRq.strApp << " | status " << stateToString(appRq.eState) << endl;
        nIndex++;
    }

    bRet = bFinished;
    EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - End" << endl;

    return bRet;
}
bool SM_UserAPP::isFinishedKO(std::string strService, std::string strIp)
{
    bool bRet;
    int nIndex;

    bRet = false;
    nIndex = findRequestIndex(strService, strIp);

    if (nIndex < getAppArraySize())
    {
        bRet = (getApp(nIndex).eState == appFinishedTimeout || getApp(nIndex).eState == appFinishedError);
    }
    else
    {
        EV_INFO << "SM_UserAPP::setStartTime::Application request not found!!!" << endl;
        printUserAPP();
    }

    return bRet;
}
bool SM_UserAPP::allAppsFinished(std::string strVmId)
{
    APP_Request appRq;
    int nIndex;
    bool bFinished;

    bFinished = true;
    nIndex = 0;

    while (bFinished && nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        if (appRq.vmId.compare(strVmId) == 0)
        {
            bFinished = (appRq.eState == appFinishedOK || appRq.eState == appFinishedTimeout || appRq.eState == appFinishedError);
        }

        nIndex++;
    }

    return bFinished;
}
void SM_UserAPP::abortAllApps(std::string strVmId)
{
    APP_Request appRq;
    int nIndex;
    bool bFinished;

    bFinished = true;
    nIndex = 0;

    while (nIndex < getAppArraySize())
    {
        appRq = getApp(nIndex);
        if (appRq.vmId.compare(strVmId) == 0)
        {
            bFinished = (appRq.eState == appFinishedOK || appRq.eState == appFinishedTimeout || appRq.eState == appFinishedError);

            if (!bFinished)
            {
                getApp(nIndex).eState = appFinishedTimeout;
                getApp(nIndex).finishTime = simTime().dbl();

                // cancelEvent(getApp(nIndex).pMsgTimeout);

                EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                        << " - Application[" << nIndex << "] " << getApp(nIndex).strApp
                        << " in VM " << strVmId
                        << " has been aborted by timeout" << endl;
                increaseFinishedApps();
            }
        }
        nIndex++;
    }
    printUserAPP();
}
bool SM_UserAPP::allAppsFinished()
{
    bool bRet;

    bRet = (nFinishedApps >= getAppArraySize());

    return bRet;
}

void SM_UserAPP::setEndTime(std::string strService, std::string strIp, double dTime)
{
    int nIndex;

    nIndex = findRequestIndex(strService, strIp);

    if (nIndex < getAppArraySize())
    {
        getApp(nIndex).finishTime = dTime;
    }
    else
    {
        EV_INFO << "SM_UserAPP::setEndTime Application request not found!!!" << endl;
        printUserAPP();
    }
}

int SM_UserAPP::addAppRequest(APP_Request appReq)
{
    // Check if a resize is necessary
    if (idCounter >= appArraySize)
    {
        // Makes sure it's harder and harder to need a realloc
        setAppArraySize(appArraySize + APP_REALLOC_SIZE_STEP);
    }

    // Insert the request counter
    unsigned int id = idCounter;
    setApp(id, appReq);
    idCounter++;
    
    return id;
}
void SM_UserAPP::printUserAPP()
{
    int nRequests, nResponses;
    APP_Request appRQ;

    // Print Message
    nRequests = getAppArraySize();
    EV_INFO << "User received: " << getUserID() << endl;
    EV_INFO << "Associated VM: " << getVmId() << endl;
    // EV_INFO << "Finished: " << this->getNFinishedApps() << endl;
    // EV_INFO << "All: " << this->allAppsFinished() << endl;
    // EV_INFO << "Ok: " << this->allAppsFinishedOK() << endl;
    // EV_INFO << "Trace: " << this->trace.empty() << endl;
    EV_INFO << "Total requests received: " << nRequests << endl;

    for (int i = 0; i < nRequests; i++)
    {
        appRQ = getApp(i);
        EV_INFO << "+RQ(" << i << "): App: " << appRQ.strApp << " | status: " << stateToString(appRQ.eState) << " | Ip:" << appRQ.strIp << " | VmId: " << appRQ.vmId << " | startTime: " << appRQ.startTime << " | endTime: " << appRQ.finishTime << endl;
    }
}
std::string SM_UserAPP::stateToString(tApplicationState eState)
{
    std::string strRet;
    switch (eState)
    {
    case appWaiting:
        strRet = "appWaiting";
        break;
    case appRunning:
        strRet = "appRunning";
        break;
    case appFinishedOK:
        strRet = "appFinishedOK";
        break;
    case appFinishedTimeout:
        strRet = "appFinishedTimeout";
        break;
    case appFinishedError:
        strRet = "appFinishedError";
        break;
    default:
        strRet = "unknown state";
    }

    return strRet;
}

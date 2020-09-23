/*
 * SM_UserAPP.cc
 *
 *  Created on: Apr 7, 2017
 *      Author: pablo
 */

#include "SM_UserAPP.h"
#include "Management/utils/LogUtils.h"

SM_UserAPP::SM_UserAPP() {
    // TODO Auto-generated constructor stub
    nFinishedApps=0;


}

SM_UserAPP::~SM_UserAPP() {
    // TODO Auto-generated destructor stub
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
int SM_UserAPP::createNewAppRequestFull(std::string strApp, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish* pMsgTimeout)
{
    APP_Request appRQ;

    appRQ.startTime=startTime;
    appRQ.strIp=strIp;
    appRQ.strApp=strApp;
    appRQ.strAppType="";
    appRQ.eState = eState;
    appRQ.vmId=vmId;

    if(pMsgTimeout != nullptr)
    {
        appRQ.pMsgTimeout = new SM_UserAPP_Finish();
        appRQ.pMsgTimeout->setUserID(pMsgTimeout->getUserID());
        appRQ.pMsgTimeout->setStrVmId(pMsgTimeout->getStrVmId());
        appRQ.pMsgTimeout->setStrApp(pMsgTimeout->getStrApp());
        appRQ.pMsgTimeout->setNTotalTime(pMsgTimeout->getNTotalTime());
    }
    else
        appRQ.pMsgTimeout=nullptr;
    //appRQ.vmId=strVmId;

   // EV_INFO << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
   //         " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequestFull(std::string strApp, std::string strAppType, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish* pMsgTimeout)
{
    APP_Request appRQ;

    appRQ.startTime=startTime;
    appRQ.finishTime=finishTime;
    appRQ.strIp=strIp;
    appRQ.strApp=strApp;
    appRQ.strAppType=strAppType;
    appRQ.eState = eState;
    appRQ.vmId=vmId;

    if(pMsgTimeout != nullptr)
    {
        appRQ.pMsgTimeout = new SM_UserAPP_Finish();
        appRQ.pMsgTimeout->setUserID(pMsgTimeout->getUserID());
        appRQ.pMsgTimeout->setStrVmId(pMsgTimeout->getStrVmId());
        appRQ.pMsgTimeout->setStrApp(pMsgTimeout->getStrApp());
        appRQ.pMsgTimeout->setNTotalTime(pMsgTimeout->getNTotalTime());
    }
    else
        appRQ.pMsgTimeout=nullptr;
    //appRQ.vmId=strVmId;

   // EV_INFO << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
   //         " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequest(std::string strService,std::string strIp,std::string strVmId,int nStartRentTime)
{
    APP_Request appRQ;

    appRQ.startTime=nStartRentTime;
    appRQ.finishTime = 0;
    appRQ.strIp=strIp;
    appRQ.strApp=strService;
    appRQ.strAppType="";
    appRQ.eState = appWaiting;
    appRQ.pMsgTimeout = nullptr;
    appRQ.vmId=strVmId;

    EV_DEBUG << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
            " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}
int SM_UserAPP::createNewAppRequest(std::string strService,std::string strAppType,std::string strIp,std::string strVmId,int nStartRentTime)
{
    APP_Request appRQ;

    appRQ.startTime=nStartRentTime;
    appRQ.finishTime = 0;
    appRQ.strIp=strIp;
    appRQ.strApp=strService;
    appRQ.strAppType=strAppType;
    appRQ.eState = appWaiting;
    appRQ.pMsgTimeout = nullptr;
    appRQ.vmId=strVmId;

    EV_DEBUG << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId << " | startTime: " << appRQ.startTime <<
            " | endTime: " << appRQ.finishTime <<endl;

    return addAppRequest(appRQ);
}

SM_UserAPP* SM_UserAPP::dup() const
{
    SM_UserAPP* pRet;

    pRet= new SM_UserAPP();
    pRet->setVmId(getVmId());
    pRet->setUserID(getUserID());
    pRet->setFinished(getFinished());
    pRet->setNFinishedApps(getNFinishedApps());

    for(int i=0 ; i < getAppArraySize (); i++)
      {
        APP_Request appReq = getApp (i);
        pRet->createNewAppRequestFull (appReq.strApp, appReq.strAppType, appReq.strIp, appReq.vmId, appReq.startTime, appReq.finishTime, appReq.eState, appReq.pMsgTimeout);
      }

    // Reserve memory to trace!
    pRet->setTraceArraySize (getTraceArraySize());

    // Copy trace!
    for (int i=0; i<trace.size(); i++)
        pRet->addNodeTrace (trace[i].first, trace[i].second);

    return pRet;
}

SM_UserAPP* SM_UserAPP::dup(std::string strVmId) const
{
    SM_UserAPP* pRet;
    int finished = 0;

    pRet= new SM_UserAPP();
    pRet->setVmId(strVmId.c_str());
    pRet->setUserID(getUserID());
    pRet->setFinished(getFinished());

    for(int i=0 ; i < getAppArraySize (); i++)
      {
        APP_Request appReq = getApp (i);
        if (strVmId.compare(appReq.vmId) == 0) {
            pRet->createNewAppRequestFull (appReq.strApp, appReq.strAppType, appReq.strIp, appReq.vmId, appReq.startTime, appReq.finishTime, appReq.eState, appReq.pMsgTimeout);
            if (appReq.eState == appFinishedOK || appReq.eState == appFinishedTimeout || appReq.eState == appFinishedError)
                finished++;
        }

      }

    pRet->setNFinishedApps(finished);

    // Reserve memory to trace!
    pRet->setTraceArraySize (getTraceArraySize());

    // Copy trace!
    for (int i=0; i<trace.size(); i++)
        pRet->addNodeTrace (trace[i].first, trace[i].second);

    return pRet;
}

void SM_UserAPP::update(SM_UserAPP* newData)
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
                    if(appReq2.pMsgTimeout != nullptr) getApp(i).pMsgTimeout = appReq2.pMsgTimeout;
                    break;
                  }
              }
          }
        appReq = getApp(i);
        if (appReq.eState == appFinishedOK || appReq.eState == appFinishedTimeout || appReq.eState == appFinishedError)
            totalFinished++;
      }

    // Reserve memory to trace!
    //setTraceArraySize (getTraceArraySize() + newData->getTraceArraySize());

    // Copy trace!
    //for (int i = 0; i < newData->trace.size(); i++)
    //    addNodeTrace(newData->trace[i].first, newData->trace[i].second);

    setNFinishedApps(totalFinished);
}

int SM_UserAPP::findRequestIndex(std::string strService, std::string strVmId)
{
    APP_Request appRq;
    int nIndex, nRet;
    bool bFound;

    bFound =  false;
    nIndex = 0;

    while(!bFound && nIndex<getAppArraySize())
    {
        appRq=getApp(nIndex);
        if(appRq.strApp.compare(strService)== 0 && strVmId.compare(appRq.vmId) == 0)
        {
            nRet = nIndex;
            bFound=true;
        }
        nIndex++;
    }
    if(!bFound)
    {
        EV_INFO << "SM_UserAPP::findRequestIndex::Application request not found, app: "<< strService << " | vmId: "<<strVmId<< endl;
    }
    return nRet;
}
void SM_UserAPP::changeState(std::string strService,std::string strVmId, tApplicationState eNewState)
{
    APP_Request* pAppRq;
    int nIndex;

    nIndex = findRequestIndex(strService, strVmId);

    if(nIndex<getAppArraySize())
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
    if(nIndex<getAppArraySize())
    {
        //Use the ip to compare
        if(strIp.compare(getApp(nIndex).strIp)==0)
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
    APP_Request* pAppRq;

    if(nIndex<getAppArraySize())
    {
        if(strService.compare(getApp(nIndex).strApp)==0)
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
void SM_UserAPP::setStartTime(std::string strService,std::string strIp, double dTime)
{
    int nIndex;

    nIndex = findRequestIndex(strService, strIp);

    if(nIndex<getAppArraySize())
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

    if(nIndex != -1 && nIndex<getAppArraySize())
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

    bFinished =  true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - Init" << endl;
    while(bFinished && nIndex<getAppArraySize())
    {
        appRq = getApp(nIndex);
        if (appRq.vmId.compare(strVmId) == 0)
            bFinished = (appRq.eState == appFinishedOK);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - App " << appRq.strApp << " | status " << stateToString(appRq.eState)<< endl;
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

    bFinished =  true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - Init" << endl;
    while(bFinished && nIndex<getAppArraySize())
    {
        appRq = getApp(nIndex);
        bFinished = (appRq.eState == appFinishedOK);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedOK - App " << appRq.strApp << " | status " << stateToString(appRq.eState)<< endl;
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

    bFinished =  true;
    nIndex = 0;

    EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - Init" << endl;
    while(bFinished && nIndex<getAppArraySize())
    {
        appRq=getApp(nIndex);
        bFinished = (appRq.eState == appFinishedError || appRq.eState == appFinishedTimeout);
        EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - App " << appRq.strApp << " | status " << stateToString(appRq.eState)<< endl;
        nIndex++;
    }

    bRet = bFinished;
    EV_DEBUG << "SM_UserAPP::allAppsFinishedKO - End" << endl;

    return bRet;
}
bool SM_UserAPP::isFinishedKO(std::string strService,std::string strIp)
{
    bool bRet;
    int nIndex;

    bRet = false;
    nIndex = findRequestIndex(strService, strIp);

    if(nIndex<getAppArraySize())
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

     bFinished =  true;
     nIndex = 0;

     while(bFinished && nIndex<getAppArraySize())
     {
         appRq=getApp(nIndex);
         if(appRq.vmId.compare(strVmId)==0)
         {
             bFinished = (appRq.eState == appFinishedOK||appRq.eState == appFinishedTimeout || appRq.eState == appFinishedError);
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

     bFinished =  true;
     nIndex = 0;

     while(nIndex<getAppArraySize())
     {
         appRq = getApp(nIndex);
         if(appRq.vmId.compare(strVmId)==0)
         {
             bFinished = (appRq.eState == appFinishedOK ||appRq.eState == appFinishedTimeout || appRq.eState == appFinishedError);

             if(!bFinished)
             {
                 getApp(nIndex).eState = appFinishedTimeout;
                 getApp(nIndex).finishTime = simTime().dbl();

                 //cancelEvent(getApp(nIndex).pMsgTimeout);


                 EV_INFO << LogUtils::prettyFunc(__FILE__, __func__)
                         << " - Application[" << nIndex << "] " << getApp(nIndex).strApp
                         << " in VM "<< strVmId
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

void SM_UserAPP::setEndTime(std::string strService,std::string strIp, double dTime)
{
    int nIndex;

    nIndex = findRequestIndex(strService, strIp);

    if(nIndex<getAppArraySize())
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
    int nAppId, nTotalSize;

    nAppId=0;

    //Resize the array size, and return the id of the request
    nAppId = getAppArraySize();
    nTotalSize = nAppId+1;
    setAppArraySize(nTotalSize);

    //Insert the request
    setApp(nAppId, appReq);

    return nAppId;
}
void SM_UserAPP::printUserAPP()
{
    int nRequests, nResponses;
    APP_Request appRQ;

    //Print Message
    nRequests = getArrayAppsSize();
    EV_INFO << "User received: " << getUserID() << endl;
    EV_INFO << "Associated VM: " << getVmId() << endl;
    //EV_INFO << "Finished: " << this->getNFinishedApps() << endl;
    //EV_INFO << "All: " << this->allAppsFinished() << endl;
    //EV_INFO << "Ok: " << this->allAppsFinishedOK() << endl;
    //EV_INFO << "Trace: " << this->trace.empty() << endl;
    EV_INFO << "Total requests received: " << nRequests << endl;

    for(int i=0;i<nRequests;i++)
    {
        appRQ=getApp(i);
        EV_INFO << "+RQ("<< i <<"): App: " << appRQ.strApp << " | status: " << stateToString(appRQ.eState) << " | Ip:"<< appRQ.strIp << " | VmId: "<< appRQ.vmId  << " | startTime: " << appRQ.startTime <<
                " | endTime: " << appRQ.finishTime <<endl;
    }
}
std::string SM_UserAPP::stateToString(tApplicationState eState)
{
    std::string strRet;
    switch(eState)
    {
    case appWaiting:
        strRet="appWaiting";
        break;
    case appRunning:
        strRet="appRunning";
        break;
    case appFinishedOK:
        strRet="appFinishedOK";
        break;
    case appFinishedTimeout:
        strRet="appFinishedTimeout";
        break;
    case appFinishedError:
        strRet="appFinishedError";
        break;
    default:
        strRet ="unknown state";
    }

    return strRet;
}

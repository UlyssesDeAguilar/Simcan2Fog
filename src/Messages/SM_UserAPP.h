/*
 * SM_UserAPP.h
 *
 *  Created on: Apr 7, 2017
 *      Author: pablo
 */

#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include "SM_UserAPP_m.h"

class SM_UserAPP: public SM_UserAPP_Base {
public:
    SM_UserAPP();
    virtual ~SM_UserAPP();

    void printUserAPP();
    int createNewAppRequestFull(std::string strApp, std::string strAppType, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish* pMsgTimeout);
    int createNewAppRequest(std::string strService, std::string strAppType, std::string strIp, std::string strVmId, int nStartRentTime);
    int createNewAppRequestFull(std::string strApp, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish* pMsgTimeout);
    int createNewAppRequest(std::string strService, std::string strIp, std::string strVmId, int nStartRentTime);
    int addAppRequest(APP_Request appReq);
    int getArrayAppsSize(){return getAppArraySize();};
    void increaseFinishedApps(){nFinishedApps++;};
    void decreaseFinishedApps(){nFinishedApps--;};

    void changeStateByIndex(int nIndex, std::string strService, tApplicationState eNewState);
    void changeState(std::string strService,std::string strIp, tApplicationState eNewState);
    void setStartTime(std::string strService,std::string strIp, double dTime);
    void setEndTime(std::string strService,std::string strIp, double dTime);

    bool isFinishedOK(std::string strService,std::string strIp);
    bool isFinishedKO(std::string strService,std::string strIp);
    bool allAppsFinished();
    bool allAppsFinishedOK();
    bool allAppsFinishedKO();
    bool allAppsFinished(std::string strVmId);
    bool allAppsFinishedOK(std::string strVmId);
    void abortAllApps(std::string strVmId);
    std::string stateToString(tApplicationState eState);

    //TODO: Esto hay que quitarlo en un futuro, cuando arreglemos la excepcion
    void setVmIdByIndex(int nIndex, std::string strIp, std::string strVmId);
    virtual SM_UserAPP* dup() const;
    virtual SM_UserAPP* dup(std::string strVmId) const;

    virtual void update(SM_UserAPP* newData);

private:
    //APP_Request* findRequest(std::string strService,std::string strIp);
    int findRequestIndex(std::string strService,std::string strIp);
};

#endif /* SM_USERAPP_H_ */

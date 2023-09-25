#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include "SM_UserAPP_m.h"
#define APP_REALLOC_SIZE_STEP 10
/**
 * @brief Class that describes a user app deployment request
 * @details Updated by Ulysses de Aguilar Gudmundsson based
 * onto the work of the author.
 * @author Pablo Cerro Cañizares
 * @version 1.1
 * @date 24-09-2023
 */
class SM_UserAPP : public SM_UserAPP_Base
{
private:
    // APP_Request* findRequest(std::string strService,std::string strIp);
    void copy(const SM_UserAPP &other);

    /**
     * @brief Copies the given App request
     * @details Watch out, it duplicates the pMsgTimeout for avoiding ownership errors
     * @param src   Source request
     * @param dest  Destination request
     */
    void copyAppRequest(const APP_Request &src, APP_Request &dest);
    int findRequestIndex(std::string strService, std::string strIp);

protected:
    APP_Request *app;
    int appArraySize;
    int idCounter;

    /**
     * @brief Sets the Apps Array new size
     * @details It's protected to avoid accidental use outside the class
     * For adding new app requests one should use the createNewApp* or addAppRequest methods
     * @param size new size
     */
    virtual void setAppArraySize(unsigned int size);

    /**
     * @brief Set the App object in the specified position
     *
     * @param k Position (AppId !)
     * @param app App request
     */
    virtual void setApp(unsigned int k, const APP_Request &app);

public:
    SM_UserAPP();
    SM_UserAPP(const SM_UserAPP &other);
    virtual ~SM_UserAPP();

    SM_UserAPP &operator=(const SM_UserAPP &other);

    void printUserAPP();
    int createNewAppRequestFull(std::string strApp, std::string strAppType, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish *pMsgTimeout);
    int createNewAppRequest(std::string strService, std::string strAppType, std::string strIp, std::string strVmId, int nStartRentTime);
    int createNewAppRequestFull(std::string strApp, std::string strIp, std::string vmId, int startTime, int finishTime, tApplicationState eState, SM_UserAPP_Finish *pMsgTimeout);
    int createNewAppRequest(std::string strService, std::string strIp, std::string strVmId, int nStartRentTime);
    int addAppRequest(APP_Request appReq);
    void increaseFinishedApps() { nFinishedApps++; };
    void decreaseFinishedApps() { nFinishedApps--; };

    unsigned int getAppArraySize() const { return idCounter; }
    APP_Request &getApp(unsigned int k);
    APP_Request &getApp(unsigned int k) const { return const_cast<SM_UserAPP *>(this)->getApp(k); };

    void changeStateByIndex(int nIndex, std::string strService, tApplicationState eNewState);
    void changeState(std::string strService, std::string strIp, tApplicationState eNewState);
    void setStartTime(std::string strService, std::string strIp, double dTime);
    void setEndTime(std::string strService, std::string strIp, double dTime);

    bool isFinishedOK(std::string strService, std::string strIp);
    bool isFinishedKO(std::string strService, std::string strIp);
    bool allAppsFinished();
    bool allAppsFinishedOK();
    bool allAppsFinishedKO();
    bool allAppsFinished(std::string strVmId);
    bool allAppsFinishedOK(std::string strVmId);
    void abortAllApps(std::string strVmId);
    std::string stateToString(tApplicationState eState);

    // TODO: Esto hay que quitarlo en un futuro, cuando arreglemos la excepcion
    void setVmIdByIndex(int nIndex, std::string strIp, std::string strVmId);
    virtual SM_UserAPP *dup() const { return new SM_UserAPP(*this); }
    virtual SM_UserAPP *dup(std::string strVmId) const;

    virtual void update(SM_UserAPP *newData);
};

#endif /* SM_USERAPP_H_ */

#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include "SM_UserAPP_m.h"
#include <vector>
#include <algorithm>

/**
 * @brief Class that describes a user app deployment request
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar
 * @version 1.2
 * @date 28-01-2024
 */
class SM_UserAPP : public SM_UserAPP_Base
{
private:
    // APP_Request* findRequest(const std::string &service,std::string strIp);
    void copy(const SM_UserAPP &other);

    /**
     * @brief Copies the given App request
     * @details Watch out, it duplicates the pMsgTimeout for avoiding ownership errors
     * @param src   Source request
     * @param dest  Destination request
     */
    void copyAppRequest(const APP_Request &src, APP_Request &dest);
    int findRequestIndex(const std::string &service, const std::string &vmId);

protected:
    std::vector<APP_Request> apps;

    /**
     * @brief Sets the Apps Array new size
     * @details It's protected to avoid accidental use outside the class
     * For adding new app requests one should use the createNewApp* or addAppRequest methods
     * @param size new size
     */
    virtual void setAppArraySize(size_t size){};

    /**
     * @brief Set the App object in the specified position
     *
     * @param k Position (AppId !)
     * @param app App request
     */
    virtual void setApp(size_t k, const APP_Request &app);

    /**
     * @brief Copies an instance request, duplicates (obviously) the timeout message also
     * @details Helper method for dup() operator
     * @param request The instance request to be duplicated
     */
    void copyAndInsertRequest(const APP_Request &request);

public:
    SM_UserAPP();
    SM_UserAPP(const SM_UserAPP &other);
    virtual ~SM_UserAPP();

    SM_UserAPP &operator=(const SM_UserAPP &other);

    void createNewAppRequest(const std::string &service, const std::string &appType, const std::string &ip, const std::string &vmId, double startRentTime);

    void increaseFinishedApps() { nFinishedApps++; };
    void decreaseFinishedApps() { nFinishedApps--; };

    size_t getAppArraySize() const { return apps.size(); }
    APP_Request &getApp(size_t k) { return apps.at(k); }
    const APP_Request &getApp(size_t k) const { return const_cast<SM_UserAPP *>(this)->getApp(k); };

    void changeState(const std::string &service, const std::string &vmId, tApplicationState eNewState);
    void changeStateByIndex(int nIndex, tApplicationState eNewState);

    // void setStartTime(const std::string &service, std::string strIp, double dTime);
    // void setEndTime(const std::string &service, const std::string &vmId, double dTime);
    void abortAllApps(const std::string &vmId);

    bool isFinishedOK(const std::string &service, const std::string &vmId);
    bool isFinishedKO(const std::string &service, const std::string &vmId);

    bool allAppsFinishedOK() { return std::all_of(apps.begin(), apps.end(), APP_Request::isFinishedOK); }
    bool allAppsFinishedKO() { return std::all_of(apps.begin(), apps.end(), APP_Request::isFinishedKO); }
    bool allAppsFinishedOK(const std::string &vmId);

    bool allAppsFinished() { return nFinishedApps >= getAppArraySize(); }
    bool allAppsFinished(const std::string &vmId);

    virtual SM_UserAPP *dup() const { return new SM_UserAPP(*this); }
    virtual SM_UserAPP *dup(const std::string &vmId) const;

    /**
     * @brief Updates the state of each individual app instance
     * #FIXME: Watch out with the possible message leaking!
     * @param newData A SM_UserAPP containing the newerstates
     */
    virtual void update(const SM_UserAPP *newData);

    /**
     * @brief Inserts and application
     * @param app APP_Request
     */
    virtual void insertApp(const APP_Request &app) { apps.emplace_back(app); };
    
    /* Extra methods, could be reused for the future*/
    virtual void insertApp(size_t k, const APP_Request &app){};
    virtual void eraseApp(size_t k){};

    friend std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj);
};

#endif /* SM_USERAPP_H_ */

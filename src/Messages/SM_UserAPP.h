#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include "SM_UserAPP_m.h"
#include "Core/include/GroupVector.hpp"
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
    void copy(const SM_UserAPP &other);

    int findRequestIndex(const std::string &service, const std::string &vmId);

protected:
    group_vector<std::string, APP_Request> vmAppGroupedVector;       //!< Apps stored and grouped by vm in a flat way
    std::vector<APP_Request> &apps = vmAppGroupedVector.flattened(); //!< Alias for quick access to the flattened version, shall not be modified

    /**
     * @brief Sets the Apps Array new size
     * @details It's protected to avoid accidental use outside the class
     * For adding new app requests one should use the createNewApp* or addAppRequest methods
     * @param size new size
     */
    virtual void setAppArraySize(size_t size) { vmAppGroupedVector.reserve(size); }

    /**
     * @brief Set the App object in the specified position
     *
     * @param k Position (AppId !)
     * @param app App request
     */
    virtual void setApp(size_t k, const APP_Request &app);

    /* Extra methods, could be reused for the future*/
    virtual void insertApp(size_t k, const APP_Request &app){};
    virtual void insertApp(const APP_Request &app){};
    virtual void eraseApp(size_t k){};

public:
    SM_UserAPP();
    SM_UserAPP(const SM_UserAPP &other);
    virtual ~SM_UserAPP();

    SM_UserAPP &operator=(const SM_UserAPP &other);

    void increaseFinishedApps() { nFinishedApps++; };
    void decreaseFinishedApps() { nFinishedApps--; };

    size_t getAppArraySize() const { return vmAppGroupedVector.size(); }
    APP_Request &getApp(size_t k) { return vmAppGroupedVector.flattened().at(k); }
    const APP_Request &getApp(size_t k) const { return const_cast<SM_UserAPP *>(this)->getApp(k); };

    void changeState(const std::string &service, const std::string &vmId, tApplicationState eNewState);
    void changeStateByIndex(int nIndex, tApplicationState eNewState);

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
    
    typedef typename group_vector<std::string, APP_Request>::iterator iterator;

    iterator begin() { return vmAppGroupedVector.begin(); }
    iterator end() { return vmAppGroupedVector.end(); }

    std::ptrdiff_t getDeploymentIndex(const std::vector<APP_Request>::iterator& it) { return std::distance(apps.begin(), it);}

    /**
     * @brief Updates the state of each individual app instance
     * #FIXME: Watch out with the possible message leaking!
     * @param newData A SM_UserAPP containing the newerstates
     */
    virtual void update(const SM_UserAPP *newData);

    friend std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj);
    friend class UserAPPBuilder;
};

class UserAPPBuilder
{
private:
    SM_UserAPP *app = nullptr;
    std::map<std::string, std::vector<APP_Request>> prev_map;

public:
    void newRequest()
    {
        if (app)
            delete app;
        app = new SM_UserAPP();
    }

    void createNewAppRequest(const std::string &service, const std::string &appType, const std::string &ip, const std::string &vmId, double startRentTime)
    {
        APP_Request appRQ;

        appRQ.startTime = startRentTime;
        appRQ.finishTime = 0;
        appRQ.strIp = ip;
        appRQ.strApp = service;
        appRQ.strAppType = appType;
        appRQ.eState = appWaiting;
        appRQ.vmId = vmId;

        EV_DEBUG << "+RQ(new): App: " << appRQ.strApp << " | status: " << appRQ.eState << " | Ip:" << appRQ.strIp << " | VmId: " << appRQ.vmId << " | startTime: " << appRQ.startTime << " | endTime: " << appRQ.finishTime << '\n';

        // The [] operator creates the std::pair if the element searched by key does not exist
        prev_map[vmId].push_back(appRQ);
    }

    SM_UserAPP *finish()
    {
        bool first = true;

        // Push data in compact and organized way
        for (const auto &elem : prev_map)
        {
            if (first)
            {
                app->vmAppGroupedVector.at(0) = elem.first;
                first = false;
            }
            else
                app->vmAppGroupedVector.new_collection(elem.first);

            for (const auto &appRequest : elem.second)
                app->vmAppGroupedVector.emplace_back(appRequest);
        }

        // Clear the previous map and mark as finished
        prev_map.clear();

        // Null inner reference
        auto retApp = app;
        app = nullptr;

        return retApp;
    }
};

#endif /* SM_USERAPP_H_ */

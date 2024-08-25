#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include <algorithm>

#include "s2f/architecture/net/routing/RoutingInfo_m.h"
#include "s2f/core/include/GroupVector.hpp"
#include "s2f/messages/SM_UserAPP_m.h"

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
    group_vector<std::string, AppRequest> vmAppGroupedVector;       //!< Apps stored and grouped by vm in a flat way
    std::vector<AppRequest> &apps = vmAppGroupedVector.flattenedForUpdate(); //!< Alias for quick access to the flattened version, shall not be modified

    /**
     * @brief Sets the Apps Array new size
     * @details It's protected to avoid accidental use outside the class
     * For adding new app requests one should use the createNewApp* or addAppRequest methods
     * @param size new size
     */
    virtual void setAppArraySize(size_t size) override { vmAppGroupedVector.reserve(size); }

    /**
     * @brief Set the App object in the specified position
     *
     * @param k Position (AppId !)
     * @param app App request
     */
    virtual void setApp(size_t k, const AppRequest &app) override;

    /* Extra methods, could be reused for the future*/
    virtual void insertApp(size_t k, const AppRequest &app) override {};
    virtual void appendApp(const AppRequest& app) override {};
    virtual void eraseApp(size_t k) override {};

public:
    SM_UserAPP();
    SM_UserAPP(const SM_UserAPP &other);
    virtual ~SM_UserAPP();

    SM_UserAPP &operator=(const SM_UserAPP &other);

    void increaseFinishedApps() { nFinishedApps++; };
    void decreaseFinishedApps() { nFinishedApps--; };

    size_t getAppArraySize() const override{ return vmAppGroupedVector.size(); }
    AppRequest &getApp(size_t k) { return vmAppGroupedVector.flattenedForUpdate().at(k); }
    const AppRequest &getApp(size_t k) const override{ return const_cast<SM_UserAPP *>(this)->getApp(k); };

    void changeState(const std::string &service, const std::string &vmId, tApplicationState eNewState);
    void changeStateByIndex(int nIndex, tApplicationState eNewState);

    void abortAllApps(const std::string &vmId);

    bool isFinishedOK(const std::string &service, const std::string &vmId);
    bool isFinishedKO(const std::string &service, const std::string &vmId);

    bool allAppsFinishedOK() { return std::all_of(apps.begin(), apps.end(), AppRequest::isFinishedOK); }
    bool allAppsFinishedKO() { return std::all_of(apps.begin(), apps.end(), AppRequest::isFinishedKO); }
    bool allAppsFinishedOK(const std::string &vmId);

    bool allAppsFinished() { return nFinishedApps >= getAppArraySize(); }
    bool allAppsFinished(const std::string &vmId);

    virtual SM_UserAPP *dup() const override { return new SM_UserAPP(*this); }
    virtual SM_UserAPP *dup(const std::string &vmId) const;

    group_vector<std::string, AppRequest>::iterator begin() { return vmAppGroupedVector.begin(); }
    group_vector<std::string, AppRequest>::iterator end() { return vmAppGroupedVector.end(); }

    std::ptrdiff_t getDeploymentIndex(const std::vector<AppRequest>::iterator &it) { return std::distance(apps.begin(), it); }

    /**
     * @brief Updates the state of each individual app instance
     * #FIXME: Watch out with the possible message leaking!
     * @param newData A SM_UserAPP containing the newerstates
     */
    virtual void update(const SM_UserAPP *newData);

    friend std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj);
    friend class UserAppBuilder;
};

class UserAppBuilder
{
private:
    using VmIdAppsMap = std::map<std::string, std::vector<AppRequest>>;
    std::map<std::string, VmIdAppsMap> requests;

public:
    void newRequest()
    {
        requests.clear();
    }

    void createNewAppRequest(const std::string &service, const std::string &appType, const std::string &ip, const std::string &vmId, double startRentTime)
    {
        AppRequest appRQ;

        appRQ.startTime = startRentTime;
        appRQ.finishTime = 0;
        appRQ.serviceName = service;
        appRQ.appType = appType;
        appRQ.state = appWaiting;

        EV_DEBUG << "+RQ(new): App: " << appRQ.serviceName
                 << " | status: " << appRQ.state
                 << " | startTime: " << appRQ.startTime
                 << " | endTime: " << appRQ.finishTime << '\n';

        // The [] operator creates the std::pair if the element searched by key does not exist
        requests[ip][vmId].push_back(appRQ);
    }

    std::vector<SM_UserAPP *> *finish(const char *userId, const char *destinationTopic)
    {
        auto petitions = new std::vector<SM_UserAPP *>();

        for (const auto &ip : requests)
        {
            SM_UserAPP *app = new SM_UserAPP();
            auto routingInfo = new RoutingInfo();

            // Setup of the request message
            routingInfo->setDestinationUrl(ServiceURL(ip.first));
            app->setControlInfo(routingInfo);
            app->setDestinationTopic(destinationTopic);
            app->setUserId(userId);
            app->setIsResponse(false);
            app->setOperation(SM_APP_Req);

            // Push data in compact and organized way
            for (const auto &elem : ip.second)
            {
                app->vmAppGroupedVector.new_collection(elem.first);

                for (const auto &appRequest : elem.second)
                    app->vmAppGroupedVector.emplace_back(appRequest);
            }
            petitions->push_back(app);
        }

        return petitions;
    }
};

#endif /* SM_USERAPP_H_ */

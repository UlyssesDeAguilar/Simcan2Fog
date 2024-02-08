#ifndef __APP_REQUEST
#define __APP_REQUEST

#include <iostream>
#include "Core/include/SIMCAN_types.h"
#include "SM_UserAPP_Finish_m.h"

/**
 * @brief Individual instance app request
 * @details It usually is included inside an SM_UserAPP
 */
struct APP_Request
{
    std::string strApp;
    std::string strAppType;
    std::string strIp;
    std::string vmId;
    double startTime;
    double finishTime;
    tApplicationState eState;
    //SM_UserAPP_Finish *pMsgTimeout;

    static bool isFinishedOK(const APP_Request &r) { return r.eState == appFinishedOK; }
    static bool isFinishedKO(const APP_Request &r) { return r.eState == appFinishedError || r.eState == appFinishedTimeout; }
    static bool isFinished(const APP_Request &r) { return isFinishedOK(r) || isFinishedKO(r); }
    friend std::ostream &operator<<(std::ostream &os, const APP_Request &obj);
};

/**
 * @brief This is a new model for app requests which is more compact -- will be used in the future
 */
struct AppRequest
{
    std::string serviceName;    //!< Name of the service being deployed
    std::string appType;        //!< Name of the instance
    double startTime;           //!< Start of execution
    double finishTime;          //!< End of execution
    tApplicationState state;    //!< State of the application

    static bool isFinishedOK(const APP_Request &r) { return r.eState == appFinishedOK; }
    static bool isFinishedKO(const APP_Request &r) { return r.eState == appFinishedError || r.eState == appFinishedTimeout; }
    static bool isFinished(const APP_Request &r) { return isFinishedOK(r) || isFinishedKO(r); }
    friend std::ostream &operator<<(std::ostream &os, const APP_Request &obj);
};

#endif /*APP_REQUEST*/

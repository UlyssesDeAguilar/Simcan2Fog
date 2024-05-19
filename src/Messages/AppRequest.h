#ifndef __APP_REQUEST
#define __APP_REQUEST

#include <iostream>
#include "Core/include/SIMCAN_types.h"

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

    static bool isFinishedOK(const AppRequest &r) { return r.state == appFinishedOK; }
    static bool isFinishedKO(const AppRequest &r) { return r.state == appFinishedError || r.state == appFinishedTimeout; }
    static bool isFinished(const AppRequest &r) { return isFinishedOK(r) || isFinishedKO(r); }
    friend std::ostream &operator<<(std::ostream &os, const AppRequest &obj);
};

#endif /*APP_REQUEST*/

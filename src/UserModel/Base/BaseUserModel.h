#ifndef SIMCAN_EX_BASE_USER_MODEL
#define SIMCAN_EX_BASE_USER_MODEL

#include "UserModel/common.h"

class UserGenerator_simple;

class BaseUserModel
{
protected:
    UserGenerator_simple &driver; //!< Reference to the driver of the simulation

    void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    void handleResponseSubscription(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    void handleResponseAppRequest(SM_UserAPP *appRequest, CloudUserInstance &userInstance);

    // Helpers
    void deployApps(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    bool decidesToRescueVm(SM_UserAPP *appRequest, CloudUserInstance &userInstance);
    void updateVmsStatus(CloudUserInstance &userInstance, const std::string &vmId, tVmState stateNew);

    // FIX ME: Not a priority, but really should check the "ephemeral" messages
    void deleteIfEphemeralMessage(SIMCAN_Message *msg);

public:
    friend class UserGenerator_simple;
    BaseUserModel(UserGenerator_simple &driver) : driver(driver) {}
};

#endif
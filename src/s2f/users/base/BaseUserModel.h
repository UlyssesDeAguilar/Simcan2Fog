#ifndef SIMCAN_EX_BASE_USER_MODEL
#define SIMCAN_EX_BASE_USER_MODEL

#include "s2f/users/common.h"

class UserGenerator_simple;

class BaseUserModel
{
protected:
    UserGenerator_simple &driver; //!< Reference to the driver of the simulation

    virtual void handleVmExtendRequest(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance);
    virtual void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    virtual void handleResponseSubscription(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    virtual void handleResponseAppRequest(SM_UserAPP *appRequest, CloudUserInstance &userInstance);

    // Helpers
    void deployApps(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    virtual bool decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance);

    // FIX ME: Not a priority, but really should check the "ephemeral" messages
    void deleteIfEphemeralMessage(SIMCAN_Message *msg);

public:
    friend class UserGenerator_simple;
    BaseUserModel(UserGenerator_simple &driver) : driver(driver) {}
    virtual ~BaseUserModel() = default;
};

#endif
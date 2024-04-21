#ifndef SIMCAN_EX_CLOUD_USER_MODEL
#define SIMCAN_EX_CLOUD_USER_MODEL

#include "UserModel/Base/BaseUserModel.h"

class CloudUserModel : public BaseUserModel
{
protected:
    virtual void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance) override;

    // Helpers
    bool decidesToRescueVm(SM_UserAPP *appRequest, CloudUserInstance &userInstance);
public:
    CloudUserModel(UserGenerator_simple &driver) : BaseUserModel(driver) {}
};

#endif
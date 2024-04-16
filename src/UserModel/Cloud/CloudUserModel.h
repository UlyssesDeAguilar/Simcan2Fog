#ifndef SIMCAN_EX_CLOUD_USER_MODEL
#define SIMCAN_EX_CLOUD_USER_MODEL

#include "UserModel/Base/BaseUserModel.h"

class CloudUserModel : public BaseUserModel
{
protected:
    void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance);
    void handleResponseAppRequest(SM_UserAPP *appRequest, CloudUserInstance &userInstance);

    // Helpers
    bool decidesToRescueVm(SM_UserAPP *appRequest, CloudUserInstance &userInstance);
public:
    CloudUserModel(UserGenerator_simple &driver) : BaseUserModel(driver) {}
};

#endif
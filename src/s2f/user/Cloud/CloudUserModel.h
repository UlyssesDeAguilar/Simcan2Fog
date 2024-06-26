#ifndef SIMCAN_EX_CLOUD_USER_MODEL
#define SIMCAN_EX_CLOUD_USER_MODEL

#include "../../user/Base/BaseUserModel.h"

class CloudUserModel : public BaseUserModel
{
protected:
    virtual void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance) override;

    // Helpers
    virtual bool decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance) override;
public:
    CloudUserModel(UserGenerator_simple &driver) : BaseUserModel(driver) {}
};

#endif
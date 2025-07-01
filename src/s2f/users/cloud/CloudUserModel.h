#ifndef SIMCAN_EX_CLOUD_USER_MODEL_H__
#define SIMCAN_EX_CLOUD_USER_MODEL_H__

#include "s2f/users/base/BaseUserModel.h"

namespace s2f
{
    namespace users
    {
        class CloudUserModel : public BaseUserModel
        {
        protected:
            virtual void handleResponseVmRequest(SM_UserVM *vmRequest, CloudUserInstance &userInstance) override;

            // Helpers
            virtual bool decidesToRescueVm(SM_VmExtend *extensionOffer, CloudUserInstance &userInstance) override;

        public:
            CloudUserModel(UserGenerator_simple &driver) : BaseUserModel(driver) {}
        };
    }
}

#endif /* SIMCAN_EX_CLOUD_USER_MODEL_H__ */
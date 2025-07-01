#ifndef SIMCAN_EX_BASE_USER_MODEL_H__
#define SIMCAN_EX_BASE_USER_MODEL_H__

#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/management/dataClasses/Users/CloudUserInstance.h"
#include "s2f/management/dataClasses/Users/CloudUserInstancePriority.h"

namespace s2f
{
    namespace users
    {
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

        public:
            friend class UserGenerator_simple;
            BaseUserModel(UserGenerator_simple &driver) : driver(driver) {}
            virtual ~BaseUserModel() = default;
        };
    }
}

#endif /* SIMCAN_EX_BASE_USER_MODEL_H__*/
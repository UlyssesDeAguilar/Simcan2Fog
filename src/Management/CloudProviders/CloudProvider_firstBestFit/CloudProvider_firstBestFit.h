#ifndef __SIMCAN_2_0_CLOUDPROVIDERFIRSTBESTFIT_H_
#define __SIMCAN_2_0_CLOUDPROVIDERFIRSTBESTFIT_H_

#include <algorithm>

#include "Management/CloudProviders/CloudProviderBase_firstBestFit/CloudProviderBase_firstBestFit.h"
#include "Management/parser/DataCenterReservationListParser.h"
#include "Management/dataClasses/DataCenterInfoCollectionReservation.h"
#include "Messages/SM_UserVM_Cost_m.h"



/**
 * Class that parses information about the data-centers.
 *
 */
class CloudProvider_firstBestFit : public CloudProviderBase_firstBestFit{

    protected:

        /** Vector that contains the types of slas generated in the current simulation */
        std::vector<Sla*> slaTypes;

//        std::vector<SM_UserVM_Finish*> pendingVmRentTimeoutMessages;

        /** Destructor*/
        ~CloudProvider_firstBestFit();

        /** Initialize the cloud provider*/
        virtual void initialize();

        virtual void initializeDataCenterCollection() override;

        virtual void initializeRequestHandlers() override;

        virtual void parseConfig() override;

        /**
         * Parses each sla type used in the simulation. These slas are allocated in the <b>slaTypes</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseSlasList();

        /**
         * Parses each user type used in the simulation. These users are allocated in the <b>userTypes</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseUsersList() override;

        /**
         * Converts the parsed sla into string format.
         *
         * @return A string containing the parsed slas.
         */
        std::string slasToString();

        /**
         * Search for a specific type of CloudUser.
         *
         * @param userType Type of a user.
         * @return If the requested type of user is located in the userTypes vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
         */
         Sla* findSla (std::string slaType);

        virtual void loadNodes() override;

        virtual int parseDataCentersList() override;

        /**
         * Check if the user request fits in the datacenter
         * @param userVM_Rq User request.
         */
//        virtual bool checkVmUserFit(SM_UserVM*& userVM_Rq) override;

        //virtual void handleExecVmRentTimeout(cMessage *msg) override;

        virtual void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);

        virtual void handleEndVmAndAbortExecution(SIMCAN_Message *sm);


//
//        virtual void abortAllApps(SM_UserAPP* userApp, std::string strVmId) override;
//
//        virtual void cancelAndDeleteAppFinishMsgs(SM_UserAPP* userApp, std::string strVmId);
//
//        virtual void handleAppExecEndSingle(SM_UserAPP_Finish* pUserAppFinish) override;
//
//        /**
//         * Handles the timeout execution of a VM rent.
//         * @param pUserVmFinish Finished VM message.
//         */
//        virtual void handleExecVmRentTimeout(SM_UserVM_Finish* pUserVmFinish) override;
//
//        /**
//         * Sends a timeout of VM renting
//         * @param userAPP_Rq apps User submission.
//         */
//        void timeoutAppRunningRequest(SM_UserAPP* userAPP_Rq);
//
//        /**
//         * Handle a user APP request.
//         * @param userAPP_Rq User APP request.
//         */
//        virtual void handleUserAppRequest(SM_UserAPP* userAPP_Rq) override;
//
//        virtual void extendVmAndResumeApp(SM_UserAPP* userAPP_Rq);
//
//        virtual void endVmAndAbortApp(SM_UserAPP* userAPP_Rq);
//
//        virtual void scheduleRentingTimeout(VM_Request& vmRequest, std::string strUsername);
//
//        virtual void checkPendingVmRentTimeoutMessages();
};

#endif

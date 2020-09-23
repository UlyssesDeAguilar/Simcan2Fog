#ifndef __SIMCAN_2_0_CLOUDPROVIDERFIRSTFITSINGLE_H_
#define __SIMCAN_2_0_CLOUDPROVIDERFIRSTFITSINGLE_H_

#include "Management/CloudProviders/CloudProviderBase/CloudProviderBase.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include "NodeUser.h"

struct VM_HashSearch
{
    //request
    std::string strUser;

    //List of different options offered by the servers
    std::vector <int> vmList;
};

/**
 *
 * Class that parses information about the data-centers.
 *
 */
class CloudProviderFirstFitSingleUser : public CloudProviderBase{

    protected:

        std::map<std::string, VM_HashSearch> userRqMap;
        std::vector<NodeUser*> nodeUsers;
        std::vector<SM_UserVM*> subscribeQueue;

        std::vector<int> initialTimes;

        bool bFinished;
        int nFreeNodes;

        ~CloudProviderFirstFitSingleUser();
        virtual void initialize();

        /**
         * Process a self message.
         * @param msg Self message.
         */
         virtual void processSelfMessage (cMessage *msg);

         /**
         * Process a request message.
         * @param sm Request message.
         */
         virtual void processRequestMessage (SIMCAN_Message *sm);

         /**
         * Process a response message from a module in the local node.
         * @param sm Response message.
         */
         virtual void processResponseMessage (SIMCAN_Message *sm);

         /**
          * Check if the user request fits in the datacenter
          * @param userVM_Rq User request.
          */
         bool checkVmUserFit(SM_UserVM*& userVM_Rq);

         /**
          * Check if the user request fits in the datacenter
          * @param userVM_Rq User request.
          */
         bool checkAppUserFit(SM_UserAPP*& userAPP_Rq);

         /**
          * Accept the user request
          * @param userVM_Rq VMs User request.
          */
         void  acceptVmRequest(SM_UserVM* userVM_Rq);

         /**
          * Accept the app request
          * @param userAPP_Rq apps User submission.
          */
         void  acceptAppRequest(SM_UserAPP* userAPP_Rq);

         /**
          * Reject the user request
          * @param userVM_Rq User request.
          */
         void rejectVmRequest(SM_UserVM* userVM_Rq);

         /**
          * Reject the user application request
          * @param userAPP_Rq User app submission.
          */
         void rejectAppRequest(SM_UserAPP* userAPP_Rq);

         void notifySubscription(SM_UserVM* userVM_Rq);

         void timeoutSubscription(SM_UserVM* userVM_Rq);

         void storeVmSubscribe(SM_UserVM* userVM_Rq);

         void loadNodes();

         void handleUserAppRequest(SM_UserAPP* userAPP_Rq);

         int getPriceByVmType(std::string strPrice);

         void freeUserVms(std::string strUsername);

         void updateSubsQueue();
};

#endif

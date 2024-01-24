#ifndef __SIMCAN_2_0_CLOUDPROVIDERCOST_H_
#define __SIMCAN_2_0_CLOUDPROVIDERCOST_H_

#include <algorithm>

#include "../../dataClasses/DataCentreInfoCollectionReservation.h"
#include "../../parser/DataCentreReservationListParser.h"
#include "../CloudProviderFirstFit/CloudProviderFirstFit.h"
#include "Messages/SM_UserVM_Cost_m.h"
#include "Management/dataClasses/Users/CloudUserPriority.h"

/**
 * Class that parses information about the data-centres.
 *
 */
class CloudProviderCost : public CloudProviderFirstFit
{

protected:
        /** Destructor*/
        ~CloudProviderCost(){};

        /** Initialize the cloud provider*/
        virtual void initialize() override;

        virtual void initializeDataCentreCollection() override;

        virtual void initializeRequestHandlers() override;

        virtual void loadNodes() override;

        virtual int parseDataCentresList() override;

        /**
         * Check if the user request fits in the datacentre
         * @param userVM_Rq User request.
         */
        //        virtual bool checkVmUserFit(SM_UserVM*& userVM_Rq) override;

        // virtual void handleExecVmRentTimeout(cMessage *msg) override;
        void handleVmRequestFits(SIMCAN_Message *sm);
        virtual void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);
        virtual void handleEndVmAndAbortExecution(SIMCAN_Message *sm);
};

#endif

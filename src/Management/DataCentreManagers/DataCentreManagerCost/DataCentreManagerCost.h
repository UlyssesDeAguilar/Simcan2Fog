#ifndef __SIMCAN_2_0_DATACENTREMANAGERCOST_H_
#define __SIMCAN_2_0_DATACENTREMANAGERCOST_H_

#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"
//#include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerCost: public DataCentreManagerFirstFit{


    protected:

        bool checkReservedFirst;
        std::map<int, std::vector<Hypervisor*>> mapHypervisorPerNodesReserved;

        /** Vector that contains the types of slas generated in the current simulation */
        std::vector<Sla*> slaTypes;

        ~DataCentreManagerCost();
        virtual void initialize();
        virtual void initializeRequestHandlers() override;
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

        virtual void parseConfig() override;

        virtual int parseDataCentreConfig () override;

        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest) override;
        virtual Hypervisor* selectNodeReserved (SM_UserVM_Cost*& userVM_Rq, const VM_Request& vmRequest);

        virtual int initDataCentreMetadata () override;

        virtual  int storeReservedNodeMetadata(cModule *pNodeModule);

        virtual void handleExecVmRentTimeout(cMessage *msg) override;

        virtual void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);

        virtual void handleEndVmAndAbortExecution(SIMCAN_Message *sm);

        Sla* findSla (std::string slaType);
};

#endif

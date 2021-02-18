#ifndef __SIMCAN_2_0_DATACENTERMANAGERCOST_H_
#define __SIMCAN_2_0_DATACENTERMANAGERCOST_H_

#include "Management/DataCenterManagers/DataCenterManager/DataCenterManager.h"
#include "Management/dataClasses/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCenterReservationConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
#include "Messages/SM_UserVM_Cost_m.h"
//#include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-center manager for cloud environments
 */
class DataCenterManagerCost: public DataCenterManager{


    protected:

        bool checkReservedFirst;
        std::map<int, std::vector<Hypervisor*>> mapHypervisorPerNodesReserved;

        /** Vector that contains the types of slas generated in the current simulation */
        std::vector<Sla*> slaTypes;

        ~DataCenterManagerCost();
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

        virtual int parseDataCenterConfig () override;

        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest) override;
        virtual Hypervisor* selectNodeReserved (SM_UserVM_Cost*& userVM_Rq, const VM_Request& vmRequest);

        virtual int initDataCenterMetadata () override;

        virtual  int storeReservedNodeMetadata(cModule *pNodeModule);

        virtual void handleExecVmRentTimeout(cMessage *msg) override;

        virtual void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);

        virtual void handleEndVmAndAbortExecution(SIMCAN_Message *sm);
};

#endif

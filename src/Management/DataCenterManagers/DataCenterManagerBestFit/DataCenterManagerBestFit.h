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
class DataCenterManagerBestFit: public DataCenterManager{


    protected:



        ~DataCenterManagerBestFit();
        virtual void initialize();

        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest) override;
        virtual void deallocateVmResources(std::string strVmId) override;
        virtual void storeNodeInMap (Hypervisor* pHypervisor);
        virtual void removeNodeFromMap (Hypervisor* pHypervisor);
};

#endif

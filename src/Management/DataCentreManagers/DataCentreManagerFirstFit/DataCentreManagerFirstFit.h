#ifndef __SIMCAN_2_0_DATACENTREMANAGERFIRSTFIT_H_
#define __SIMCAN_2_0_DATACENTREMANAGERFIRSTFIT_H_

#include "../DataCentreManagerBase/DataCentreManagerBase.h"
#include "Management/dataClasses/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCentreReservationConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
#include "Messages/SM_UserVM_Cost_m.h"
//#include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerFirstFit: public DataCentreManagerBase{

    public:
        virtual void initialize();
    protected:



        ~DataCentreManagerFirstFit();


        virtual Hypervisor* selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest) override;
};

#endif

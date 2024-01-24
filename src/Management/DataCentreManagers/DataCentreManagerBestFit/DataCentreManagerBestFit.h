#ifndef __SIMCAN_2_0_DATACENTREMANAGERCOST_H_
#define __SIMCAN_2_0_DATACENTREMANAGERCOST_H_

#include "../DataCentreManagerBase/DataCentreManagerBase.h"
#include "Management/dataClasses/Users/CloudUserInstance.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/parser/DataCentreReservationConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
#include "Messages/SM_UserVM_Cost_m.h"
#include "Management/utils/LogUtils.h"

// #include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerBestFit : public DataCentreManagerBase
{

protected:
    virtual ~DataCentreManagerBestFit() {};
    virtual void initialize() override;

    virtual Hypervisor *selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest) override;
    virtual void deallocateVmResources(std::string strVmId) override;
    virtual void storeNodeInMap(Hypervisor *pHypervisor);
    virtual void removeNodeFromMap(Hypervisor *pHypervisor);
};

#endif

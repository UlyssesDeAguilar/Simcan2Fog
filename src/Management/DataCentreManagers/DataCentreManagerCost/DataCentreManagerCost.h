#ifndef __SIMCAN_2_0_DATACENTREMANAGERCOST_H_
#define __SIMCAN_2_0_DATACENTREMANAGERCOST_H_

#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"
#include "Management/dataClasses/Users/CloudUserPriority.h"
// #include "Applications/UserApps/LocalApplication/LocalApplication.h"

/**
 * Module that implementa a data-centre manager for cloud environments
 */
class DataCentreManagerCost : public DataCentreManagerFirstFit
{
protected:
    bool checkReservedFirst;
    std::map<int, std::vector<Hypervisor *>> mapHypervisorPerNodesReserved;

    ~DataCentreManagerCost() {};
    virtual void initialize() override;
    virtual void initializeRequestHandlers() override;
    virtual int parseDataCentreConfig() override;
    virtual int initDataCentreMetadata() override;
    virtual int storeReservedNodeMetadata(cModule *pNodeModule);
    virtual Hypervisor *selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest) override;
    virtual Hypervisor *selectNodeReserved(SM_UserVM_Cost *&userVM_Rq, const VM_Request &vmRequest);
    virtual void handleExecVmRentTimeout(cMessage *msg) override;
    virtual void handleExtendVmAndResumeExecution(SIMCAN_Message *sm);
    virtual void handleEndVmAndAbortExecution(SIMCAN_Message *sm);
};

#endif

#ifndef SIMCAN_EX_VM_CONTROL_TABLE_H__
#define SIMCAN_EX_VM_CONTROL_TABLE_H__

#include <omnetpp.h>
#include "s2f/os/control/ControlEntries.h"

class VmControlTable : public omnetpp::cSimpleModule
{
protected:
    std::map<omnetpp::opp_string, int> globalToLocalVmIdMap; //!< Map that translates the general VM Id to the local VM Id
    std::map<int, omnetpp::opp_string> localToGlobalVmIdMap; //!< Map that translates the local VM Id to the general VM Id
    std::vector<VmControlBlock> vmTable;                     //!< Control table for vms
    cMessage *timeoutMsg;                                    //!< Timeout counter, every tick is a second
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

public:
    VmControlTable();
    virtual ~VmControlTable();

    const VmControlBlock &getVmControlBlock(int vmId) const;
    const AppControlBlock &getAppControlBlock(int vmId, int appId) const;

    void createMapping(const char *globalVmId, int localVmId);
    const char *getGlobalVmId(int localVmId) { return localToGlobalVmIdMap.at(localVmId).c_str(); }
    int getLocalVmId(const char *globalVmId) { return globalToLocalVmIdMap.at(globalVmId); }
    int getPidFromServiceName(int vmId, const char *serviceName);
    
    int allocateVm(const VirtualMachine *vmType = nullptr, const char *userId = nullptr);
    void deallocateVm(int vmId);
    void associateDeploymentToVm(int vmId, SM_UserAPP *request);
    
    void suspendVm(int vmId);
    bool vmIsSuspended(int vmId);
    void bufferCall(int vmId, cMessage *msg);
    cQueue &resumeVm(int vmId);

    int allocateApp(int vmId, int deploymentIndex = 0);
    void deallocateApp(int vmId, int appId);
};

#endif /* SIMCAN_EX_VM_CONTROL_TABLE_H__ */
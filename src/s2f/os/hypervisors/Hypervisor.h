#ifndef SIMCAN_EX_HYPERVISOR_H__
#define SIMCAN_EX_HYPERVISOR_H__

#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/os/hypervisors/common.h"
#include "s2f/os/schedulers/CpuSchedulerRR.h"
#include "s2f/os/control/VmControlTable.h"
#include "s2f/architecture/net/protocol/L2Protocol_m.h"
#include "s2f/messages/SM_AppSettings_m.h"

namespace hypervisor
{
    class Hypervisor : public cSimpleModule
    {
    protected:
        HardwareManager *hardwareManager; //!< Reference to the hardwareManager
        DataManager *dataManager;         //!< Reference to the dataManager
        VmControlTable *controlTable;     //!< Virtual machine control table
        ApplicationBuilder appBuilder;    //!< Application builder
        uint32_t maxAppsPerVm;            //!< Max number of vms per vm -> TODO: Check out the values

        GateInfo appGates;       //!< General info for appGates
        GateInfo schedulerGates; //!< General info for schedulerGates
        GateInfo networkGates;   //!< General info for network gates

        virtual void handleVmTimeout(VmControlBlock &vm) { error("Default vm timeout not implemented"); }

        // Subclass interface section

        /**
         * @brief Get the Application Module from the topology
         * @param vmId The virtual machine ID
         * @param pid  The process ID
         * @return cModule* Pointer to the AppModule module
         */
        virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) = 0;
        virtual CpuSchedulerRR *getScheduler(uint32_t vmId) = 0;

        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(cMessage *msg) override;

        virtual void processSyscallStart(Syscall *request);
        virtual void processSyscallEnd(Syscall *request);
        virtual void processCommand(SIMCAN_Message *sm);

        virtual void sendEvent(cMessage *sm) = 0;
        virtual void sendToNetwork(cMessage *msg, int destination);
        virtual void sendOrBufferToApp(Syscall *syscall);
        virtual void sendToApp(cMessage *msg, int vmId, int pid);

        virtual bool messageIsCommand(SIMCAN_Message *sm);
        virtual void handleAppTermination(int pid, int vmId, tApplicationState exitStatus);
        virtual void handleAppRequest(SM_UserAPP *sm);
        virtual void handleAppSettings(SM_AppSettings *sm);
        virtual void handleVmRequest(SM_UserVM *sm);
        virtual void handleVmExtension(SM_VmExtend *msg);
        virtual void deallocateVmResources(int vmId);
    };
};

#endif /*SIMCAN_EX_HYPERVISOR_H__*/

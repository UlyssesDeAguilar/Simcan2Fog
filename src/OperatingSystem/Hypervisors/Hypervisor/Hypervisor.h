#ifndef SIMCAN_EX_HYPERVISOR
#define SIMCAN_EX_HYPERVISOR

#include "OperatingSystem/Hypervisors/common.h"
#include "OperatingSystem/Hypervisors/OsCore/OsCore.h"
#include "OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.h"
#include "Management/dataClasses/Applications/Application.h"

namespace hypervisor
{
    class Hypervisor : public cSIMCAN_Core
    {
    protected:
        enum InitStage
        {
            INNER_STAGE,
            LOCAL_STAGE
        };

        struct GateInfo
        {
            int inBaseId;  //!< Base Id of the input vector
            int outBaseId; //!< Base Id of the output vector
        };

        HardwareManager *hardwareManager; //!< Reference to the hardwareManager
        DataManager *dataManager;
        OsCore osCore;                           //!< The core operating system utilities
        GateInfo appGates;                       //!< General info for appGates
        GateInfo schedulerGates;                 //!< General info for schedulerGates
        ControlTable<VmControlBlock> vmsControl; //!< Control table for vms
        uint32_t maxAppsPerVm;                   //!< Max number of vms per vm -> TODO: Check out the values
        std::map<std::string, uint32_t> vmIdMap; //!< Map that translates the general VM Id to the local VM Id

        
        uint32_t takePid(uint32_t vmId) { return vmsControl[vmId].apps.takeId(); }
        void releasePid(uint32_t vmId, uint32_t pid) { vmsControl[vmId].apps.releaseId(pid); }

        AppControlBlock &getAppControlBlock(uint32_t vmId, uint32_t pid) { return vmsControl[vmId].apps[pid]; }
        virtual void handleAppRequest(SM_UserAPP *sm);

        // Subclass interface section

        /**
         * @brief Get the Application Module from the topology
         * @param vmId The virtual machine ID
         * @param pid  The process ID
         * @return cModule* Pointer to the AppModule module
         */
        virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) = 0;
        
        /**
         * @brief Locates the hardware manager in the simulation topology
         * @return HardwareManager* Pointer to the hardware manager for direct communication
         */
        virtual HardwareManager *locateHardwareManager() = 0;

        /**
         * @brief Translates the global vm id into the inner vmId
         * @param vmId The global VM ID
         * @return uint32_t The local vmId or UINT32_MAX if not found
         */
        virtual uint32_t resolveGlobalVmId(const std::string &vmId) = 0;

        // Inherited section
        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return 2; }

        virtual void finish() override;
        virtual cGate *getOutGate(cMessage *msg) override;
        virtual void processSelfMessage(cMessage *msg) override;
        virtual void processRequestMessage(SIMCAN_Message *sm) override;
        virtual void processResponseMessage(SIMCAN_Message *sm) override;

        // The OsCore must be able to query the context
        friend class OsCore;
    };
};

#endif /*SIMCAN_EX_HYPERVISOR*/

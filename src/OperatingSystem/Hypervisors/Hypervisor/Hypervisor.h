#ifndef SIMCAN_EX_HYPERVISOR
#define SIMCAN_EX_HYPERVISOR

#include "OperatingSystem/Hypervisors/common.h"
#include "OperatingSystem/Hypervisors/OsCore/OsCore.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.h"
#include "Management/dataClasses/Applications/Application.h"

namespace hypervisor
{
    class Hypervisor : public cSIMCAN_Core
    {
    protected:
        HardwareManager *hardwareManager;        //!< Reference to the hardwareManager
        OsCore osCore;                           //!< The core operating system utilities
        std::map<std::string, uint32_t> vmIdMap; //!< Map that translates the general VM Id to the local VM Id

        virtual AppControlBlock& getAppControlBlock(uint32_t pid) = 0;
        virtual cModule * getApplicationModule(uint32_t vmId, uint32_t pid) = 0;

        virtual uint32_t takePid() = 0;
        virtual void releasePid(uint32_t pid) = 0;

        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return 2; }

        virtual void finish() override;
        virtual cGate *getOutGate(cMessage *msg) override;
        virtual void processSelfMessage(cMessage *msg) override;
        virtual void processRequestMessage(SIMCAN_Message *sm) override;
        virtual void processResponseMessage(SIMCAN_Message *sm) override;
    
    friend class OsCore;
    };
};

#endif /*SIMCAN_EX_HYPERVISOR*/

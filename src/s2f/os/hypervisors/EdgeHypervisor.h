/**
 * @file EdgeHypervisor.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Module declaration for the Edge Hypervisor
 * @version 0.1
 * @date 2023-12-03
 *
 */
#ifndef SIMCAN_EX_EDGE_HYPERVISOR
#define SIMCAN_EX_EDGE_HYPERVISOR

#include "s2f/architecture/nodes/hardwaremanagers/HardwareManager.h"
#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/os/hypervisors/OsCore.h"
#include "s2f/os/hypervisors/common.h"
#include "s2f/os/hypervisors/Hypervisor.h"

/**
 * @brief Class that models the behaviour of the EdgeNode hypervisor
 * @details It may be considered the OS of the EdgeNode
 */
namespace hypervisor
{
    class EdgeHypervisor : public Hypervisor
    {
    protected:
        cModule *appsVector;
        const char *starterApps;
        SM_UserAPP *sam{};
        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return INITSTAGE_LAST + 1; }
        virtual HardwareManager *locateHardwareManager() override { return check_and_cast<HardwareManager *>(getModuleByPath("^.hwm")); }
        virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) override { return appsVector->getSubmodule("appModule", pid); }
        virtual uint32_t resolveGlobalVmId(const std::string &vmId) override { return 0; }
    };
}

#endif

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

#include "OperatingSystem/Hypervisors/common.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/dataClasses/Applications/Application.h"
#include "Core/cSIMCAN_Core.h"
#include "OperatingSystem/Hypervisors/OsCore/OsCore.h"

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
        virtual void initialize(int stage) override;

        virtual HardwareManager *locateHardwareManager() override { return check_and_cast<HardwareManager *>(getModuleByPath("^.hwm")); }
        virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) override { return appsVector->getSubmodule("appModule", pid); }
        virtual void handleAppRequest(SM_UserAPP *sm) override;
    };
}

#endif

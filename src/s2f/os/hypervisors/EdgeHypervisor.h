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

#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/management/dataClasses/Applications/Application.h"
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
        cModule *appsVector{};
        cGate *serialOut{};
        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return NEAR + 1; }
        virtual void sendEvent(cMessage *msg) override;
        virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) override { return appsVector->getSubmodule("appModule", pid); }
        virtual CpuSchedulerRR *getScheduler(uint32_t vmId) override { return nullptr; } 
    };
}

#endif

/**
 * @file EdgeHypervisor.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Module declaration for the Edge Hypervisor
 * @version 2.0
 * @date 2025-05-10
 *
 */
#ifndef SIMCAN_EX_EDGE_HYPERVISOR_H_
#define SIMCAN_EX_EDGE_HYPERVISOR_H_

#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/os/hypervisors/common.h"
#include "s2f/os/hypervisors/Hypervisor.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief Class that models the behaviour of the EdgeNode hypervisor
         * @details It may be considered the OS of the EdgeNode
         * @author Ulysses de Aguilar Gudmundsson
         * @version 2.0
         */
        class EdgeHypervisor : public Hypervisor
        {
        protected:
            cModule *appsVector{}; //!< Vector that holds the user applications
            cGate *serialOut{};    //!< Output gate to serial

            virtual void initialize(int stage) override;
            virtual int numInitStages() const override { return NEAR + 1; }
            virtual void sendEvent(cMessage *msg) override;
            
            virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) override { return appsVector->getSubmodule("appModule", pid); }
            virtual CpuSchedulerRR *getScheduler(uint32_t vmId) override { return nullptr; }
        };
    }
}

#endif /* SIMCAN_EX_EDGE_HYPERVISOR_H_ */

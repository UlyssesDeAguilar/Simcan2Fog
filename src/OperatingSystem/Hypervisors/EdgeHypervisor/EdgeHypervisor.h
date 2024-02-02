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
        // TODO : App timeouts ?
        // TODO : Evaluate if it belongs to a single user ? (It should)
    protected:

        ControlTable<AppControlBlock> appsControl;
        std::vector<uint32_t> freePids; // Helps keep track of the available "slots" for the apps
        uint32_t maxApps;
        uint32_t runningApps;
        cModule *appsVector;

        virtual void initialize() override;
        virtual void finish() override;
        virtual cGate *getOutGate(cMessage *msg) override;
        virtual void processSelfMessage(cMessage *msg) override;
        virtual void processRequestMessage(SIMCAN_Message *sm) override;
        virtual void processResponseMessage(SIMCAN_Message *sm) override;

        uint32_t newPid(int vmId);
        void releasePid(int vmId, int pid) { freePids.push_back(pid); }

        cModule *getApplicationModule(int vmId, int pid) { return appsVector->getSubmodule("appModule", pid); }
        AppControlBlock &getControlBlock(int vmId, int pid) { return appsControl[vmId * 0 + pid]; }
    };
}

#endif

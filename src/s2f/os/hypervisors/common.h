/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief This file contains common definitions and headers for all hypervisors
 * @version 0.1
 * @date 2023-12-05
 *
 */
#ifndef SIMCAN_EX_HYPERVISOR_COMMON
#define SIMCAN_EX_HYPERVISOR_COMMON

#include <deque>
#include <omnetpp.h>

#include "s2f/architecture/nodes/hardwaremanagers/HardwareManager.h"
#include "s2f/architecture/net/routing/RoutingInfo_m.h"
#include "s2f/core/simdata/DataManager.h"
#include "s2f/messages/INET_AppMessage.h"
#include "s2f/messages/SM_CPU_Message.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/messages/Syscall_m.h"
#include "s2f/os/AppIdLabel/AppIdLabel_m.h"
#include "ControlTable/ControlTable.hpp"
#include "s2f/apps/ApplicationBuilder.h"

// Forward declaration
class SM_Syscall;

namespace hypervisor
{
    typedef enum
    {
        VM_TIMEOUT, //!< A virtual machine reached the maximum renting time
        POWER_ON    //!< Event for powering on the machine
    } AutoEvent;

    typedef enum
    {
        OK,
        ERROR
    } SyscallResult;

    /**
     * @brief Keeps all the necessary control information for the app
     */
    struct AppControlBlock
    {
        uint32_t pid;                         //!< Process Id
        uint32_t vmId;                        //!< Group Id -- Will help to identify the VM
        tApplicationState status;             //!< The exit status (0 - OK, 1 - ERROR, 2 - FORCED_EXIT, 3 - RUNNING)
        std::map<int, ConnectionMode> sockets; //!< Map that contains the application sockets
        int deploymentIndex;                  //!< The index relative for that request

        void initialize(uint32_t pid)
        {
            this->pid = pid;
            this->vmId = 0;
            status = tApplicationState::appWaiting;
        }

        void reset()
        {
            this->vmId = 0;
        }

        bool isRunning() { return status == tApplicationState::appRunning; }
    };

    /**
     * @brief Keeps all the necessary control information for the VM
     */
    struct VmControlBlock
    {
        uint32_t vmId;                       //!< The vmId -- It's aligned in virtual environements with the scheduler!
        tVmState state;                      //!< The current state of the VM
        opp_string globalId;         //!< The unique identifier of the vm in the global context of the simulation
        const VirtualMachine *vmType;        //!< The virtual machine type
        std::string userId;                  //!< The current owner of the vm
        ControlTable<AppControlBlock> apps;  //!< The apps that are currently executing
        std::deque<Syscall *> callBuffer; //!< Holds request/responses when the vm is in suspension state
        cMessage *timeOut{};                 //!< Timeout message for vm
        SM_UserAPP *request{};               //!< The request that instantiated the app

        void initialize(uint32_t vmId)
        {
            this->vmId = vmId;
            state = tVmState::vmIdle;
            vmType = nullptr;
        }

        friend std::ostream &operator<<(std::ostream &os, const VmControlBlock &obj)
        {
            return os << "VmId:     " << obj.vmId << '\n';
        }
    };

}

#endif

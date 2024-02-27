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

#include <omnetpp.h>
#include "Core/DataManager/DataManager.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "Architecture/Network/RoutingInfo/RoutingInfo_m.h"
#include "Applications/Builder/include.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_VmExtend_m.h"
#include "Messages/INET_AppMessage.h"
#include "Messages/SM_CPU_Message.h"
#include "OperatingSystem/AppIdLabel/AppIdLabel_m.h"
#include "OperatingSystem/Hypervisors/ControlTable/ControlTable.hpp"
#include "Management/dataClasses/NodeResourceRequest.h" // Keep this temporarily for hypervisor VM requests

// Forward declaration
class SM_Syscall;

namespace hypervisor
{
    typedef enum
    {
        IO_DELAY,   //!< Input/Output delay finished
        VM_TIMEOUT, //!< A virtual machine reached the maximum renting time
        POWER_ON    //!< Event for powering on the machine
    } AutoEvent;

    typedef enum
    {
        READ,      // Read from disk
        WRITE,     // Write to disk
        EXEC,      // Execute / Calculate
        OPEN_CLI,  // Open client socket : PORT/IP
        OPEN_SERV, // Open server socket : PORT/DOMAIN/SERVICE_NAME
        SEND,      // Send through socket
        RECV,      // Recieve through socket
        EXIT,      // Finish the process
    } Syscall;

    // TODO: Consider if making the pointers smart pointers!
    struct CallContext
    {
        Syscall opCode;
        union Context
        {
            double bufferSize;          // Either for read or write
            SM_CPU_Message *cpuRequest; // For CPU requests
            struct NetIO
            {
                INET_AppMessage *packet; // For network I/O
                int fd;                  // For network I/O
            };
        } data;
    };

    struct Connection
    {
        enum Flags
        {
            BIND = 1, // If set the port is binded, ephemeral otherwise
            OPEN = 2  // If set the port/socket is open, closed otherwise
        };

        int fd;
        short flags;

        Connection(int fd, bool binded, bool open = true)
        {
            this->fd = fd;
            if (binded)
                flags |= Flags::BIND;
            if (open)
                flags |= Flags::OPEN;
        }
    };

    /**
     * @brief Keeps all the necessary control information for the app
     */
    struct AppControlBlock
    {
        uint32_t pid;                      // Process Id
        uint32_t vmId;                     // Group Id -- Will help to identify the VM
        tApplicationState status;          // The exit status (0 - OK, 1 - ERROR, 2 - FORCED_EXIT, 3 - RUNNING)
        std::map<int, Connection> sockets; // Map that contains the application sockets
        SM_UserAPP *request;               // The request that instantiated the app
        int deploymentIndex;               // The index relative for that request
        SM_Syscall *lastRequest;           // Last SYSCALL by the app

        void initialize(uint32_t pid)
        {
            this->pid = pid;
            this->vmId = 0;
            status = tApplicationState::appWaiting;
            request = nullptr;
            lastRequest = nullptr;
        }

        void reset()
        {
            this->vmId = 0;
            request = nullptr;
            lastRequest = nullptr;
        }

        bool isRunning() { return status == tApplicationState::appRunning; }
    };

    /**
     * @brief Keeps all the necessary control information for the VM
     */
    struct VmControlBlock
    {
        uint32_t vmId;                      //!< The vmId -- It's aligned in virtual environements with the scheduler!
        std::string userId;                 //!< The current owner of the vm
        const std::string *globalId;        //!< The unique identifier of the vm in the global context of the simulation
        ControlTable<AppControlBlock> apps; //!< The apps that are currently executing
        tVmState state;                     //!< The current state of the VM
        NodeResourceRequest *request;       //!< The request that allocated this VM
        // cMessage *timeOut;                  //!< The timeout event
        // SM_UserVM *msg; <-- We'll see if this makes sense

        void initialize(uint32_t vmId)
        {
            this->vmId = vmId;
            state = tVmState::vmIdle;
            request = nullptr;
            globalId = nullptr;
        }

        friend std::ostream &operator<<(std::ostream &os, const VmControlBlock &obj)
        {
            return os << "VmId:     " << obj.vmId << '\n';
        }
    };

}

// Include the Syscall message after to avoid import loop
#include "Messages/SM_Syscall_m.h"

#endif

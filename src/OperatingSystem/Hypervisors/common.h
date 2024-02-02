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
#include "Applications/Builder/include.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_UserVM.h"
#include "Messages/INET_AppMessage.h"
#include "Messages/SM_CPU_Message.h"
#include "OperatingSystem/AppIdLabel/AppIdLabel_m.h"
#include "Management/dataClasses/NodeResourceRequest.h" // Keep this temporarily for hypervisor VM requests

// Forward declaration
class SM_Syscall;

namespace hypervisor
{
    typedef enum
    {
        IO_DELAY,    //!< Input/Output delay finished
        APP_TIMEOUT, //!< Application hit timeout (Maybe more appropiate for a VM?)
        POWER_ON     //!< Event for powering on the machine
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
     * @brief Keeps all the necessary control information for the VM
     */
    struct VmControlBlock
    {
        uint32_t vmId;                //<! The vmId -- It's aligned in virtual environements with the scheduler!
        tVmState state;               //<! The current state of the VM
        NodeResourceRequest *request; //<! The request that allocated this VM
        // SM_UserVM *msg; <-- We'll see if this makes sense
        cMessage *timeOut; //<! The timeout event
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

    template <class T>
    class ControlTable
    {
        std::vector<std::pair<bool, T>> elements;
        uint32_t lastId;
        uint32_t allocatedIds;

    protected:
        /**
         * @brief Scans for a free entry
         * @details It assumes that there's at least one entry
         * @return uint32_t The first index/id that it's free
         */
        uint32_t scanFreeId()
        {
            // Filter
            const auto filter = [](std::pair<bool, T> &e) -> bool
            { return e.first };

            // Search for a free slot
            auto beginning = elements.begin();
            beginning += lastId;
            
            auto iter = std::find_if(beginning, elements.end(), filter);

            // Found going forwards
            if (iter != elements.end())
            {
                lastId = iter->second;
                return lastId;
            }

            // Found going backwards
            iter = std::find_if(elements.begin(), beginning, filter);
            lastId = iter->second;
            return lastId;
        }

    public:
        /**
         * @brief Initializes the table
         * @param size      Number of entries
         * @param init_f    Initializer function which takes the given id
         */
        void init(uint32_t size, void (T::*init_f)(uint32_t))
        {
            // Reserve memory
            elements.reserve(size);

            // Construct and give the VM id
            for (uint32_t i = 0; i < size; i++)
            {
                // Default initialize elements
                auto inserted = elements.emplace_back(true, T());

                // Call init function
                (inserted.second) * init_f(i);
            }

            // Save memory
            elements.shrink_to_fit();
        }

        uint32_t takeId()
        {
            uint32_t id = scanFreeId();
            elements[id].first = false;
            allocatedIds++;
            return id;
        }

        void releaseId(uint32_t id)
        {
            allocatedIds--;
            elements[id].first = true;
        }

        uint32_t getAllocatedIds() { return allocatedIds; }

        T &operator[](uint32_t id) { return elements[id].second; }
        T &at(uint32_t id) {return elements.at(id).second;}
    };
}

// Include the Syscall message after to avoid import loop
#include "Messages/SM_Syscall_m.h"

#endif

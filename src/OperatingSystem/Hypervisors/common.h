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
#include "Messages/SM_UserAPP.h"
#include "Messages/INET_AppMessage.h"
#include "Messages/SM_CPU_Message.h"

namespace hypervisor
{
    typedef enum
    {
        IO_DELAY,
        APP_TIMEOUT
    } AutoEvent;

    typedef enum
    {
        OK,
        ERROR,
        FORCED_EXIT,
        RUNNING
    } ExitStatus;


    typedef enum
    {
        READ,            // Read from the disk
        WRITE,           // Write from the disk
        EXEC,            // Execute / Calculate
        SEND_NETWORK,    // Send through network
        BIND_AND_LISTEN, // Bind and listen to incoming requests (maybe it's the entrypoint for service registration ?)
        EXIT,            // Finish the process
    } Syscall;
    
    // TODO: Consider if making the pointers smart pointers!
    typedef struct{
        Syscall opCode;
        union CallContext
        {
            double bufferSize;            // Either for read or write
            SM_CPU_Message  *cpuRequest;  // For CPU requests
            INET_AppMessage *packet;      // For network I/O
        };
    } CallContext;

    /**
     * @brief Keeps all the necessary control information for the app
     */
    typedef struct{
        uint32_t pid;                // Process Id
        uint32_t vmId;               // Group Id -- Will help to identify the VM
        int32_t exitStatus;          // The exit status (0 - OK, 1 - ERROR, 2 - FORCED_EXIT, 3 - RUNNING)
        APP_Request *request;        // The request that instantiated the app
        SM_Syscall *lastRequest; // Last SYSCALL by the app

        void initialize(uint32_t pid, uint32_t vmId)
        {
            this->pid = pid;
            this->vmId = vmId;
            exitStatus = ExitStatus::OK;
            request = nullptr;
            lastRequest = nullptr;
        }

        bool isRunning() { return exitStatus == ExitStatus::RUNNING; }
    } AppControlBlock;

    // TODO: Consider moving all of this functionality to the HardwareManager
    typedef struct
    {
        double totalCores;  // All cores of the CPU
        double totalMemory; // Total RAM  (In GB)
        struct Disk
        {
            double total;          // Total Disk (In GB)
            double readBandwidth;  // In Mbit/s
            double writeBandwidth; // In Mbit/s
        } disk;
    } SystemSpecs;
}
#endif

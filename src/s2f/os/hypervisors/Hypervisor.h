#ifndef SIMCAN_EX_HYPERVISOR_H__
#define SIMCAN_EX_HYPERVISOR_H__

#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/os/hypervisors/common.h"
#include "s2f/os/schedulers/CpuSchedulerRR.h"
#include "s2f/os/control/VmControlTable.h"
#include "s2f/architecture/net/protocol/L2Protocol_m.h"
#include "s2f/messages/SM_AppSettings_m.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief This model is the foundation of the different hypervisors
         * @details It's an abstract class which covers about 80% of the common hypervisor behaviour
         * 
         * @author Alberto Nuñez Covarrubias
         * @author Ulysses de Aguilar Gudmundsson
         * @version 3.0
         */
        class Hypervisor : public cSimpleModule
        {
        protected:
            HardwareManager *hardwareManager; //!< Reference to the hardwareManager
            DataManager *dataManager;         //!< Reference to the dataManager
            VmControlTable *controlTable;     //!< Virtual machine control table
            ApplicationBuilder appBuilder;    //!< Application builder
            uint32_t maxAppsPerVm;            //!< Max number of vms per vm -> TODO: Check out the values
            GateInfo appGates;                //!< General info for appGates
            GateInfo schedulerGates;          //!< General info for schedulerGates
            GateInfo networkGates;            //!< General info for network gates


            virtual void initialize() override;
            virtual void finish() override;
            virtual void handleMessage(cMessage *msg) override;

            /**
             * @brief Handles a virtual machine timeout
             * @param vm The virtual machine
             */
            virtual void handleVmTimeout(VmControlBlock &vm) { error("Default vm timeout not implemented"); }
            
            /**
             * @brief Get the Application Module from the topology
             * @param vmId The virtual machine ID
             * @param pid  The process ID
             * @return cModule* Pointer to the AppModule module
             */
            virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) = 0;

            /**
             * @brief Get the scheduler from the topology
             * @param vmId The virtual machine ID
             * @return CpuSchedulerRR* Pointer to the scheduler
             */
            virtual CpuSchedulerRR *getScheduler(uint32_t vmId) = 0;

            /**
             * @brief Process the start of a system call
             * @param request The system call
             */
            virtual void processSyscallStart(Syscall *request);

            /**
             * @brief Process the end of a system call
             * @param request The system call
             */
            virtual void processSyscallEnd(Syscall *request);

            /**
             * @brief Process a command message
             * @param sm The command message
             */
            virtual void processCommand(SIMCAN_Message *sm);

            /**
             * @brief Send an event to the control agent
             * @param sm The event message
             */
            virtual void sendEvent(cMessage *sm) = 0;

            /**
             * @brief Send a payload to the network
             * @details This method is currently intended for internal dc networks, however it can be generalized 
             * @param msg Message to be sent
             * @param destination Destination adress, e.g an L2Address
             */
            virtual void sendToNetwork(cMessage *msg, int destination);

            /**
             * @brief Sends or buffers a syscall for the application module
             * @details Buffering happens when a VM is suspended
             * @param syscall The syscall in question
             */
            virtual void sendOrBufferToApp(Syscall *syscall);

            /**
             * @brief Send a message to the application module
             * @param msg The message
             * @param vmId The virtual machine ID
             * @param pid The process ID
             */
            virtual void sendToApp(cMessage *msg, int vmId, int pid);

            /**
             * @brief Determines if the message is a command
             * 
             * @param sm The message to analyze
             * @return bool True if the message is a command 
             */
            virtual bool messageIsCommand(SIMCAN_Message *sm);

            /**
             * @brief Handles the termination of an application
             * @param pid The process ID
             * @param vmId The virtual machine ID
             * @param exitStatus The exit status
             */
            virtual void handleAppTermination(int pid, int vmId, tApplicationState exitStatus);

            /**
             * @brief Handles the request for a new application
             * @param sm The request message
             */
            virtual void handleAppRequest(SM_UserAPP *sm);
            
            /**
             * @brief Handles the request for changing the settings of an application
             * @param sm The request message
             */
            virtual void handleAppSettings(SM_AppSettings *sm);

            /**
             * @brief Handles the request for a new virtual machine
             * @param sm The request message
             */
            virtual void handleVmRequest(SM_UserVM *sm);
            
            /**
             * @brief Handles the extension of a virtual machine
             * @param sm The request message
             */
            virtual void handleVmExtension(SM_VmExtend *msg);

            /**
             * @brief Deallocates the resources associated to a virtual machine
             * @param vmId The virtual machine id
             */
            virtual void deallocateVmResources(int vmId);
        };
    }
}

#endif /*SIMCAN_EX_HYPERVISOR_H__*/

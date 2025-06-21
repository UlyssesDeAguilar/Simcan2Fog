#ifndef SIMCAN_EX_CONTROL_ENTRIES_H__
#define SIMCAN_EX_CONTROL_ENTRIES_H__

#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"
#include "s2f/messages/SM_CPU_Message.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/messages/Syscall_m.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief Control entry for apps
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        struct AppControlBlock
        {
            int pid = -1;             //!< Process Id, -1 if not allocated
            int vmId = -1;            //!< Group Id -- Will help to identify the VM
            int deploymentIndex = -1; //!< The index relative for the request that instantiated this app

            /**
             * @brief Initialize the app control block
             * @param vmId The vm id
             * @param pid The process id
             * @param deploymentIndex The index relative for the request that instantiated this app
             */
            void initialize(int vmId, int pid, int deploymentIndex)
            {
                this->pid = pid;
                this->vmId = vmId;
                this->deploymentIndex = deploymentIndex;
            }

            /**
             * @brief Free the app control block
             */
            void free() { this->pid = -1; }

            /**
             * @brief Check if the app control block is free
             */
            bool isFree() { return pid == -1; }
        };

        /**
         * @brief Keeps all the necessary control information for the VM
         */
        struct VmControlBlock
        {
            int vmId;                          //!< The vmId
            tVmState state = tVmState::vmIdle; //!< The current state of the VM
            const VirtualMachine *vmType{};    //!< The virtual machine type
            opp_string userId;                 //!< The current owner of the vm
            std::vector<AppControlBlock> apps; //!< The apps that are currently executing
            cQueue callBuffer;                 //!< Holds system calls when the vm is in suspension state
            SM_UserAPP *request{};             //!< The request that instantiated the app

            ~VmControlBlock();

            /**
             * @brief Initialize the vm control block
             * @param vmId The vm id
             * @param vmType The virtual machine type
             * @param userId The user id
             * @param apps The number of app entries to be preallocated
             */
            void initialize(int vmId, const VirtualMachine *vmType, const char *userId, size_t apps = 0);

            /**
             * @brief Resize the app control block internal vector
             * @param apps The number of app entries to be preallocated
             */
            void resize(size_t apps);

            /**
             * @brief Free the vm control block
             */
            void free();

            /**
             * @brief Set the state to running
             */
            void setRunning() { state = tVmState::vmRunning; }

            /**
             * @brief Set the state to suspended
             */
            void setSuspended() { state = tVmState::vmSuspended; }

            /**
             * @brief Check if the vm is running
             * @return True if the vm is running
             */
            bool isRunning() { return state == tVmState::vmRunning; }

            /**
             * @brief Check if the vm is suspended
             * @return True if the vm is suspended
             */
            bool isSuspended() { return state == tVmState::vmSuspended; }

            /**
             * @brief Check if the vm is free
             * @return True if the vm is free
             */
            bool isFree() { return state == tVmState::vmIdle; }

            friend std::ostream &operator<<(std::ostream &os, const VmControlBlock &obj);
        };
    }
}

#endif /* SIMCAN_EX_CONTROL_ENTRIES_H__ */
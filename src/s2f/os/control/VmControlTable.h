#ifndef SIMCAN_EX_VM_CONTROL_TABLE_H__
#define SIMCAN_EX_VM_CONTROL_TABLE_H__

#include <omnetpp.h>
#include "s2f/os/control/ControlEntries.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief Virtual machine control table model
         * @details It keeps all the context information for the VMs in a fog (includes cloud and edge) node
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class VmControlTable : public omnetpp::cSimpleModule
        {
        protected:
            std::map<omnetpp::opp_string, int> globalToLocalVmIdMap; //!< Map that translates the general VM Id to the local VM Id
            std::map<int, omnetpp::opp_string> localToGlobalVmIdMap; //!< Map that translates the local VM Id to the general VM Id
            std::vector<VmControlBlock> vmTable;                     //!< Control table for vms

            virtual void initialize() override;
            virtual void finish() override;
            virtual void handleMessage(omnetpp::cMessage *msg) override;

        public:
            /**
             * @brief Get the Vm Control Block
             *
             * @param vmId The vm id
             * @return const VmControlBlock& reference to the control block
             */
            const VmControlBlock &getVmControlBlock(int vmId) const;

            /**
             * @brief Get the App Control Block of an application in a VM
             *
             * @param vmId The vm id
             * @param appId The app id
             * @return const AppControlBlock& reference to the control block
             */
            const AppControlBlock &getAppControlBlock(int vmId, int appId) const;

            /**
             * @brief Creates a mapping between a global vm id and a local vm id
             *
             * @param globalVmId The global vm id
             * @param localVmId The local vm id
             */
            void createMapping(const char *globalVmId, int localVmId);

            /**
             * @brief Translates the local vm id to the global vm id
             *
             * @param localVmId The local vm id
             * @return const char* The global vm id
             */
            const char *getGlobalVmId(int localVmId) { return localToGlobalVmIdMap.at(localVmId).c_str(); }

            /**
             * @brief Translates the global vm id to the local vm id
             * @param globalVmId The global vm id
             * @return int The local vm id
             */
            int getLocalVmId(const char *globalVmId) { return globalToLocalVmIdMap.at(globalVmId); }

            /**
             * @brief Gets the pid from the service name
             *
             * @param vmId The vm id where the app resides
             * @param serviceName The service name
             * @return int The pid
             */
            int getPidFromServiceName(int vmId, const char *serviceName);

            /**
             * @brief Allocates a VM control block
             *
             * @param vmType The vm type
             * @param userId The user id
             * @return int The vm id
             */
            int allocateVm(const VirtualMachine *vmType = nullptr, const char *userId = nullptr);

            /**
             * @brief Deallocates a VM control block
             *
             * @param vmId The vm id
             */
            void deallocateVm(int vmId);

            /**
             * @brief Associates a deployment to a VM
             *
             * @param vmId The vm id
             * @param request The application request, otherwise known as the deployment
             */
            void associateDeploymentToVm(int vmId, SM_UserAPP *request);

            /**
             * @brief Suspends a VM
             * @param vmId The vm id
             */
            void suspendVm(int vmId);

            /**
             * @brief Checks if a VM is suspended
             */
            bool vmIsSuspended(int vmId);

            /**
             * @brief Stores a syscall request/response in an internal buffer
             * @details This method should be used to keep results for a suspended vm
             * @param vmId The vm id
             * @param msg The message
             */
            void bufferCall(int vmId, cMessage *msg);

            /**
             * @brief Resumes a suspended VM
             *
             * @param vmId The vm id
             * @return cQueue& Reference to the buffer which contains
             * the messages for the vm while it was suspended
             */
            cQueue &resumeVm(int vmId);

            /**
             * @brief Allocates a new application in a VM
             * 
             * @param vmId The vm id
             * @param deploymentIndex The index in the deployment request 
             * @return int The app's pid
             */
            int allocateApp(int vmId, int deploymentIndex = 0);

            /**
             * @brief Deallocates an application
             * 
             * @param vmId Vm id where it resides
             * @param appId The app's pid
             */
            void deallocateApp(int vmId, int appId);
        };
    }
}
#endif /* SIMCAN_EX_VM_CONTROL_TABLE_H__ */
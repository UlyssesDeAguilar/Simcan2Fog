#ifndef SIMCAN_EX_NODE_RESOURCES_H__
#define SIMCAN_EX_NODE_RESOURCES_H__

#include <stdint.h>
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief Represents the available resources of a node
         */
        struct NodeResources
        {
            uint32_t memoryMiB; //!< Total RAM  (In MiB)
            uint32_t diskMiB;   //!< Disk (In MiB)
            uint32_t cores;     //!< Cores of the CPU
            uint32_t vms;       //!< Number of VMs executed in this node
            uint32_t users;     //!< Number of users accepted in this node

            /**
             * @brief Calculates the absolute sum of resources differences
             * @details If the node does not have enough resources to allocate the VM, returns DBL_MAX (stdint.h)
             * @param vm The virtual machine of reference
             * @return double The distance
             */
            double getLogicalDistance(const VirtualMachine &vm) const
            {
                if (!hasUserAndVmSpace())
                    return DBL_MAX;

                int distance1 = cores - vm.getNumCores();
                int distance2 = diskMiB - vm.getDiskMiB();
                int distance3 = memoryMiB - vm.getMemoryMiB();

                if (distance1 < 0 || distance2 < 0 || distance3 < 0)
                    return DBL_MAX;

                return distance1 + distance2 + distance3;
            }

            /**
             * @brief Checks if the node has enough resources to allocate the VM
             * @return bool True if the node has enough resources
             */
            bool hasUserAndVmSpace() const { return (vms > 0 && users > 0); }

            /**
             * @brief Checks if the node has no more resources left
             * @return bool True if the node has no more resources
             */
            bool isExhausted() const { return (cores | memoryMiB | diskMiB) == 0 || !hasUserAndVmSpace(); }

            friend std::ostream &operator<<(std::ostream &os, const NodeResources &obj)
            {
                return os << "Memory: " << (obj.memoryMiB / 1024.0) << " "
                          << "Disk:   " << (obj.diskMiB / 1024.0) << " "
                          << "Cores:  " << obj.cores << " "
                          << "Users:  " << obj.users << " "
                          << "Vms:    " << obj.vms << " ";
            }
        };
    }
}

bool operator==(const s2f::os::NodeResources &node, const VirtualMachine &vm);

bool operator==(const VirtualMachine &vm, const s2f::os::NodeResources &node);

bool operator>=(const s2f::os::NodeResources &node, const VirtualMachine &vm);

bool operator<=(const VirtualMachine &vm, const s2f::os::NodeResources &node);

#endif /* SIMCAN_EX_NODE_RESOURCES_H__ */
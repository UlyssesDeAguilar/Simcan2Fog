#ifndef SIMCAN_EX_NODE_RESOURCES_H__
#define SIMCAN_EX_NODE_RESOURCES_H__

#include <stdint.h>
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"

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

    double getLogicalDistance(const VirtualMachine &vm) const
    {
        if (!hasUserAndVmSpace())
            return DBL_MAX;

        int distance1 = cores - vm.getNumCores();
        int distance2 = diskMiB - vm.getDiskMiB();
        int distance3 = memoryMiB - vm.getMemoryMiB();

        return std::abs(distance1) + std::abs(distance2) + std::abs(distance3);
    }

    bool hasUserAndVmSpace() const { return (vms > 0 && users > 0); }

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

bool operator==(const NodeResources &node, const VirtualMachine &vm);
bool operator==(const VirtualMachine &vm, const NodeResources &node);
bool operator>=(const NodeResources &node, const VirtualMachine &vm);
bool operator<=(const VirtualMachine &vm, const NodeResources &node);

#endif
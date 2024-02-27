#ifndef SIMCAN_EX_NODE_RESOURCES
#define SIMCAN_EX_NODE_RESOURCES

#include "Management/dataClasses/VirtualMachines/VirtualMachine.h"
#include <stdint.h>

/**
 * @brief Represents the available resources of a node
 */
struct NodeResources
{
    double memory;  //!< Total RAM  (In GB)
    double disk;    //!< Disk (In GB)
    uint32_t cores; //!< Cores of the CPU
    uint32_t vms;   //!< Number of VMs executed in this node
    uint32_t users; //!< Number of users accepted in this node

    /**
     * @brief Checks if the node resources exactly match what the VirtualMachine needs
     *
     * @param vm The element to be compared to
     * @return true If the node resources exactly match what the VirtualMachine needs
     * @return false I.o.c
     */
    bool operator==(const VirtualMachine &vm) const
    {
        bool exactMatch = true;

        // Matches exactly the needs of the vm
        exactMatch &= (cores == vm.getNumCores());
        exactMatch &= (disk == vm.getMemoryGb());
        exactMatch &= (memory == vm.getMemoryGb());

        return exactMatch && hasUserAndVmSpace();
    }

    /**
     * @brief Checks if the node resources satisfy what the VirtualMachine needs
     *
     * @param vm The element to be compared to
     * @return true If the node can satisfy the vm needs
     * @return false I.o.c
     */
    bool operator<=(const VirtualMachine &vm) const
    {
        bool lessOrEqual = true;

        // Matches exactly the needs of the vm
        lessOrEqual &= (cores <= vm.getNumCores());
        lessOrEqual &= (disk <= vm.getMemoryGb());
        lessOrEqual &= (memory <= vm.getMemoryGb());

        // Must have space for a vm or user

        return lessOrEqual && hasUserAndVmSpace();
    }

    double getLogicalDistance(const VirtualMachine &vm) const
    {
        if (!hasUserAndVmSpace())
            return DBL_MAX;

        int distance1 = cores - vm.getNumCores();
        double distance2 = disk - vm.getDiskGb();
        double distance3 = memory - vm.getMemoryGb();

        if (distance1 < 0 || distance2 < 0.0 || distance3 < 0.0)
            return DBL_MAX;

        return distance1 + distance2 + distance3;
    }

    bool hasUserAndVmSpace() const { return (vms > 0 && users > 0); }

    bool isExhausted() const
    {
        bool exhausted = false;
        exhausted |= (cores == 0);
        exhausted |= (disk == 0.0);
        exhausted |= (memory == 0.0);
        
        return exhausted || !hasUserAndVmSpace();
    }
};

#endif
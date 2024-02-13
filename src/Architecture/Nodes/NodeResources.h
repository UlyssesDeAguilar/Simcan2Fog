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
     * @brief Checks if all parameters are equal
     * 
     * @param other The element to be compared to
     * @return true If strictly all parameters are equal
     * @return false I.o.c
     */
    bool operator==(const VirtualMachine &other)
    {
        bool exactMatch = true;
        
        // Using OR doesn't mask differences by substraction
        exactMatch &= (cores == other.getNumCores());
        //TODO
    }

    /**
     * @brief Checks if all parameters are less or equal
     * 
     * @param other The element to be compared to
     * @return true If strictly all parameters are less or equal
     * @return false I.o.c
     */
    bool operator<=(const NodeResources &other)
    {
        //TODO
    }
};

#endif
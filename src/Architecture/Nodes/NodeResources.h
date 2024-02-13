#ifndef SIMCAN_EX_NODE_RESOURCES
#define SIMCAN_EX_NODE_RESOURCES

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
};

#endif
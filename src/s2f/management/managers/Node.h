#ifndef SIMCAN_EX_NODE_H_
#define SIMCAN_EX_NODE_H_

#include "s2f/os/hardwaremanagers/NodeResources.h"

struct Node
{
    enum State
    {
        ACTIVE = 1,    //!< If the hypervisor is powered on
        IN_USE = 2,    //!< If the hypervisor has active users
        MAXED_OUT = 4, //!< If the hypervisor currently cannot allocate any more vms/launch more users
    };

    s2f::os::NodeResources availableResources{}; //!< Current available resources
    int address{};                      //!< Address of the node
    uint8_t state{};                    //!< The actual state of the vm

    void setActive();
    void setInUse();
    void setMaxedOut();
    void setInactive();

    bool isActive() const;
    bool inUse() const;
    bool isMaxedOut() const;

    friend std::ostream &operator<<(std::ostream &os, const Node &node);
};

#endif

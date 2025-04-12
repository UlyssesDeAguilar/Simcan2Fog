#ifndef SIMCAN_EX_EDGE_HARDWAREMANAGER_H__
#define SIMCAN_EX_EDGE_HARDWAREMANAGER_H__

#include "s2f/os/hardwaremanagers/HardwareManager.h"

class EdgeHardwareManager : public HardwareManager
{
protected:
    int nodeIp;              //!< The node Ip
    NodeResources available; //!< Available HW/SW resources
    NodeResources total;     //!< Specifications of the HW/SW resources
    virtual void initialize();

public:
    virtual bool tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex) override;
    virtual void deallocateResources(const VirtualMachine *vmTemplate, const uint32_t *coreIndex) override;
    virtual const NodeResources &getTotalResources() const override { return total; }
    virtual const NodeResources &getAvailableResources() const override { return available; }
    virtual int getIp() const override { return nodeIp; }
};

#endif /* SIMCAN_EX_EDGE_HARDWAREMANAGER_H__ */
#ifndef SIMCAN_EX_EDGE_HARDWAREMANAGER_H__
#define SIMCAN_EX_EDGE_HARDWAREMANAGER_H__

#include "s2f/os/hardwaremanagers/HardwareManager.h"
namespace s2f
{
    namespace os
    {
        /**
         * @brief Implementation of the HardwareManager class for an edge node
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class EdgeHardwareManager : public HardwareManager
        {
        protected:
            int nodeIp;              //!< The node Ip
            NodeResources available; //!< Available HW/SW resources
            NodeResources total;     //!< Specifications of the HW/SW resources

            virtual void initialize();
        public:
            // Implementation of the parent's abstract methods

            virtual bool tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex) override;
            virtual void deallocateResources(const VirtualMachine *vmTemplate, const uint32_t *coreIndex) override;
            virtual const NodeResources &getTotalResources() const override { return total; }
            virtual const NodeResources &getAvailableResources() const override { return available; }
            virtual int getIp() const override { return nodeIp; }
        };
    }
}
#endif /* SIMCAN_EX_EDGE_HARDWAREMANAGER_H__ */
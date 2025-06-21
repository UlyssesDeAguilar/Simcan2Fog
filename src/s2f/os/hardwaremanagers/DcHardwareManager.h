#ifndef SIMCAN_EX_EDGE_HARDWAREMANAGER_H__
#define SIMCAN_EX_EDGE_HARDWAREMANAGER_H__

#include "s2f/management/managers/ResourceManager.h"
#include "s2f/os/hardwaremanagers/HardwareManager.h"
#include "s2f/architecture/net/card/NetworkCard.h"
#include "s2f/core/include/SIMCAN_types.h"

namespace s2f
{
    namespace os
    {
        /**
         * @brief Implementation of the HardwareManager class for a cloud node
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class DcHardwareManager : public HardwareManager
        {
        protected:
            NodeResources total;
            ResourceManager *resourceManager{};
            NetworkCard *networkCard{};
            size_t nodeId;

            virtual void initialize(int stage) override;
            virtual int numInitStages() const override { return SimCanInitStages::BLADE + 1; }

        public:
            virtual bool tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex) override;
            virtual void deallocateResources(const VirtualMachine *vmTemplate, const uint32_t *coreIndex) override;
            virtual const NodeResources &getTotalResources() const override;
            virtual const NodeResources &getAvailableResources() const override;
            virtual int getIp() const override;
        };
    }
}

#endif /* SIMCAN_EX_EDGE_HARDWAREMANAGER_H__ */
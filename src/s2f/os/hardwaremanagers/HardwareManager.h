#ifndef __SIMCAN_2_0_HARDWAREMANAGER_H_
#define __SIMCAN_2_0_HARDWAREMANAGER_H_

#include <omnetpp.h>
#include "s2f/os/hardwaremanagers/NodeResources.h"

namespace s2f
{
  namespace os
  {
    /**
     * @brief This class manages the hardware resources of a node
     *
     * @author Pablo Cerro Cañizares
     * @author Ulysses de Aguilar Gudmundsson
     * @version 2.0
     */
    class HardwareManager : public omnetpp::cSimpleModule
    {
    public:
      struct CoreState
      {
        int64_t usageTime = 0; //!< The accumulated usage time (seconds)
        int64_t startTime = 0; //!< Start time of current use (seconds)
        bool free = true;      //!< If the core itself is free (on start it is)
      };

    protected:
      bool isVirtualHardware;            //!< Indicates if this module is able to virtualize hardware
      std::vector<CoreState> coresState; //!< Keeps track of the state of the cores

      virtual void initialize() override;
      virtual void finish() override;
      virtual void handleMessage(omnetpp::cMessage *msg) override;
      
      /**
       * @brief Checks if the node has the right settings in place
       * @param resources The node resources to be checked
       */
      virtual void checkNodeSetup(const NodeResources &resources);

      /**
       * @brief Allocates the given number of cores
       * @param cores The number of cores to allocate
       * @param coreIndex The index indicating which cores were allocated
       */
      virtual void allocateCores(int cores, uint32_t **const coreIndex);

      /**
       * @brief Deallocates the given number of cores
       * @param coreIndex The index indicating which cores were allocated
       * @param coreIndexSize The size of the index
       */
      virtual void deallocateCores(const uint32_t *coreIndex, size_t coreIndexSize);

    public:
      /**
       * @brief Attempts to allocate resources
       *
       * @param vmTemplate The virtual machine template, defines the resources
       * @param coreIndex The index indicating which cores were allocated (on error it's set to nullptr)
       * @return true If the resources where allocated
       * @return false If the resources where unable to be allocated
       */
      virtual bool tryAllocateResources(const VirtualMachine *vmTemplate, uint32_t **const coreIndex) = 0;

      /**
       * @brief Deallocates resources
       *
       * @param vmTemplate The virtual machine template, defines the resources
       * @param coreIndex The index indicating which cores were allocated
       */
      virtual void deallocateResources(const VirtualMachine *vmTemplate, const uint32_t *coreIndex) = 0;

      /**
       * @brief Gets the total resources of the node
       * @return const Resources&
       */
      virtual const NodeResources &getTotalResources() const = 0;

      /**
       * @brief Gets the available resources of the node
       * @return const Resources&
       */
      virtual const NodeResources &getAvailableResources() const = 0;

      /**
       * @brief Get the Ip address
       * @return uint32_t (Ipv4 address)
       */
      virtual int getIp() const = 0;
    };
  }
}

#endif

#ifndef __SIMCAN_2_0_HARDWAREMANAGER_H_
#define __SIMCAN_2_0_HARDWAREMANAGER_H_

#include <omnetpp.h>
#include "Architecture/Nodes/NodeResources.h"

using namespace omnetpp;

/**
 * @brief This class manages the resources given a node
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 */
class HardwareManager : public cSimpleModule
{

public:
  struct CoreState
  {
    int64_t usageTime = 0; //!< The accumulated usage time (seconds)
    int64_t startTime = 0; //!< Start time of current use (seconds)
    bool free = true;      //!< If the core itself is free (on start it is)
  };

  

private:
  uint32_t nodeIp;                   //!< The node Ip
  NodeResources available;           //!< Available HW/SW resources
  NodeResources total;               //!< Specifications of the HW/SW resources
  bool isVirtualHardware;            //!< Indicates if this module is able to virtualize hardware
  std::vector<CoreState> coresState; //!< Keeps track of the state of the cores

protected:
  virtual void initialize() override;
  virtual void finish() override;
  virtual void handleMessage(cMessage *msg) override;

public:
  /**
   * @brief Attempts to allocate resources
   *
   * @param cores Number of cores requested
   * @param memory Memory requested (GB)
   * @param disk Disk requested (GB)
   * @param coreIndex The index indicating which cores were allocated (on error it's set to nullptr)
   * @return true If the resources where allocated
   * @return false If the resources where unable to be allocated
   */
  bool tryAllocateResources(const uint32_t &cores, const double &memory, const double &disk, uint32_t **const coreIndex);

  /**
   * @brief Deallocates resources
   *
   * @param cores Number of cores requested
   * @param memory Memory requested (GB)
   * @param disk Disk requested (GB)
   * @param coreIndex The index indicating which cores were allocated
   */
  void deallocateResources(const uint32_t &cores, const double &memory, const double &disk, const uint32_t *coreIndex);

  /**
   * @brief Gets the total resources of the node
   * @return const Resources&
   */
  const NodeResources &getTotalResources() const { return total; }

  /**
   * @brief Gets the available resources of the node
   * @return const Resources&
   */
  const NodeResources &getAvailableResources() const { return available; }

  /**
   * @brief Get the Ip address
   * @return uint32_t (Ipv4 address)
   */
  const uint32_t getIp() const { return nodeIp; }

  /**
   * @brief Whether it is a node that can virtualize hardware or not
   * @return true If it's able
   * @return false In other case
   */
  bool isVirtual() { return isVirtualHardware; }
};

#endif

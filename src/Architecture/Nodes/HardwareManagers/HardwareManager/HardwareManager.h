#ifndef __SIMCAN_2_0_HARDWAREMANAGER_H_
#define __SIMCAN_2_0_HARDWAREMANAGER_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * @brief This class manages the resources given a node
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 */
class HardwareManager : public cSimpleModule
{

public:
  struct Resources
  {
    double memory;  //!< Total RAM  (In GB)
    double disk;    //!< Disk (In GB)
    uint32_t cores; //!< Cores of the CPU
    uint32_t vms;   //!< Number of VMs executed in this computer
    uint32_t users; //!< Number of users executed in this computer
  };

  struct DiskSpecs
  {
    double readBandwidth;  //!< In Mbit/s
    double writeBandwidth; //!< In Mbit/s
  } diskSpecs;

private:
  Resources available;     //<! Available HW/SW resources
  Resources total;         //<! Specifications of the HW/SW resources
  bool isVirtualHardware;  //<! Indicates if this module is able to virtualize hardware
  bool *freeCoresArrayPtr; //<! Keeps track of the free cores in the system

protected:
  virtual void initialize() override;
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
  bool tryAllocateResources(const uint32_t &cores, const double &memory, const double &disk, const uint32_t **coreIndex);

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
  const Resources &getTotalResources() { return total; }

  /**
   * @brief Gets the available resources of the node
   * @return const Resources&
   */
  const Resources &getAvailableResources() { return available; }

  /**
   * @brief Gets the disk specifications object
   * @return const DiskSpecs&
   */
  const DiskSpecs &getDiskSpecs() { return diskSpecs; }

  /**
   * @brief Whether it is a node that can virtualize hardware or not
   * @return true If it's able
   * @return false In other case
   */
  bool isVirtual() { return isVirtualHardware; }

  // FIXME
  bool *getFreeCoresArrayPtr() const { return freeCoresArrayPtr; };
};

#endif

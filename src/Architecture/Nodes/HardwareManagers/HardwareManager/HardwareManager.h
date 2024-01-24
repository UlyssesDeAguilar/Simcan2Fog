#ifndef __SIMCAN_2_0_HARDWAREMANAGER_H_
#define __SIMCAN_2_0_HARDWAREMANAGER_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class HardwareManager : public cSimpleModule
{

public:
  int getAvailableCores();
  int getNumCores();
  int getAvailableRam();

  unsigned int *allocateCores(int numCores);
  bool allocateRam(double memory);
  bool allocateDisk(double disk);
  int deallocateCores(int numCores, unsigned int *cpuCoreIndex);
  double deallocateRam(double memory);
  double deallocateDisk(double disk);
  bool *getFreeCoresArrayPtr() const;
  double getAvailableDisk() const;
  void setAvailableDisk(double availableDisk);

private:
  /** Indicates if this module is able to virtualize hardware */
  bool isVirtualHardware;

  /** Number of CPU cores managed by this module */
  unsigned int numCpuCores;

  unsigned int numAvailableCpuCores;

  /** Maximum number of VMs executed in this computer */
  unsigned int maxVMs;

  /** Maximum number of users executed in this computer */
  unsigned int maxUsers;

  bool *freeCoresArrayPtr;

  double memorySize;
  double availableMemory;
  double diskSize;
  double availableDisk;

  //
  //                               /** Array to assign CPU scheduler to different users */
  //                               bool *cpuSchedulerUtilization;
  //
  //       /** List of users currently executing apps in this computer */
  //       UserExecution** userVector;

protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg);
};

#endif

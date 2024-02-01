#ifndef __SIMCAN_2_0_HARDWARE_REDIRECTOR_H_
#define __SIMCAN_2_0_HARDWARE_REDIRECTOR_H_

#include "Core/cSIMCAN_Core.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/dataClasses/Applications/Application.h"

class Hypervisor : public cSIMCAN_Core
{

protected:
   HardwareManager *hardwareManager;
   cMessage *powerMessage;
   int nPowerOnTime; //<! Time to power on

   // THESE CAN BE EXTRACTED FROM HW MANAGER
   bool isVirtualHardware; //<! Indicates if this module is able to virtualize hardware
   unsigned int maxVMs;    //<! Maximum number of VMs allocated in this computer
   //        unsigned int numAllocatedVms;

   // INSTEAD OF THIS USING BASE + INDEX = ID
   cGate **fromAppsGates; /** Input gate from Apps. */
   cGate **toAppsGates;   /** Output gate to Apps. */
   cGate **fromCPUGates;  /** Input gate from CPU. */
   cGate **toCPUGates;    /** Output gate to CPU. */

   cModule **pAppsVectorsArray;
   cModule *pAppsVectors;
   cModule **pCpuSchedArray;
   cModule *pCpuScheds;

   bool *freeSchedArray;

   std::map<string, int> mapVmScheduler;
   std::map<std::string, NodeResourceRequest *> mapResourceRequestPerVm;

   void setActive(bool active) { par("active").setBoolValue(active); }

   virtual ~Hypervisor();
   virtual void initialize() override;
   virtual void finish() override;
   virtual cGate *getOutGate(cMessage *msg) override;
   virtual void processSelfMessage(cMessage *msg) override;
   virtual void processRequestMessage(SIMCAN_Message *sm) override;
   virtual void processResponseMessage(SIMCAN_Message *sm) override;

public:
   int getAvailableCores() const { return hardwareManager->getAvailableResources().cores; }
   int getNumCores() const { return hardwareManager->getTotalResources().cores; }
   bool *getFreeCoresArrayPtr() const { return hardwareManager->getFreeCoresArrayPtr(); }
   bool isActive() const { return par("isActive"); }

   // BOTH OF THESE SHOULD BE HIDDEN -- Interfacing via requests!
   cModule *allocateNewResources(NodeResourceRequest *pResourceRequest);
   void deallocateVmResources(std::string strVmId);

   /**
    * Check if there are VMs running in the machine.
    * @return True if a VM is running in the machine. False otherwise.
    */
   bool isInUse();
   void powerOn(bool active);
};

#endif

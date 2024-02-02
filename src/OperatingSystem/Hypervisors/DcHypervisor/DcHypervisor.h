#ifndef SIMCAN_EX_DC_HYPERVISOR
#define SIMCAN_EX_DC_HYPERVISOR

#include "OperatingSystem/Hypervisors/common.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.h"
#include "Management/dataClasses/Applications/Application.h"

namespace hypervisor
{
   class DcHypervisor : public cSIMCAN_Core
   {
   private:
      void loadVector(std::vector<cModule *> &v, cModule *osModule, cModule *(*accessor)(cModule *, int));

   protected:
      HardwareManager *hardwareManager; //!< Reference to the hardwareManager
      cMessage *powerMessage;           //!< Power on event
      int nPowerOnTime;                 //!< Time to power on (in seconds)

      struct GateInfo
      {
         int inBaseId;  //!< Base Id of the input vector
         int outBaseId; //!< Base Id of the output vector
      };

      GateInfo appGates;                       //!< General info for appGates
      GateInfo schedulerGates;                 //!< General info for schedulerGates
      ControlTable<VmControlBlock> vmsControl; //!< Control table for vms

      // std::vector<cModule *> appVectors;
      std::vector<cModule *> schedulers;

      // bool *freeSchedArray;

      std::map<std::string, uint32_t> vmIdMap; //!< Map that translates the general VM Id to the local VM Id

      void setActive(bool active) { par("active").setBoolValue(active); }

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
      bool isInUse() { return vmsControl.getAllocatedIds() > 0; }
      void powerOn(bool active);
   };
};

#endif /*SIMCAN_EX_DC_HYPERVISOR*/

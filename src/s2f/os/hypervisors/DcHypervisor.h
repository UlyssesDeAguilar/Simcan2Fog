#ifndef SIMCAN_EX_DC_HYPERVISOR_H__
#define SIMCAN_EX_DC_HYPERVISOR_H__

#include "s2f/os/schedulers/CpuSchedulerRR.h"
#include "s2f/os/hypervisors/common.h"
#include "s2f/os/hypervisors/Hypervisor.h"
#include "s2f/architecture/net/db/SimpleNetDb.h"

namespace hypervisor
{
   class DcHypervisor : public Hypervisor
   {
   protected:
      cMessage *powerMessage{};                 //!< Power on event
      std::vector<CpuSchedulerRR *> schedulers; //!< Vector that contains the managed schedulers
      SimpleNetDb *netDb{};                     //!< Network database

      void setActive(bool active) { par("active").setBoolValue(active); }

      virtual void initialize(int stage) override;
      virtual int numInitStages() const override { return NEAR + 1; }
      virtual void finish() override;

      // public:
      bool isActive() const { return par("isActive"); }
      virtual void handleVmTimeout(VmControlBlock &vm) override;

      // Hypervisor inheritance section
      virtual cModule *getApplicationModule(uint32_t vmId, uint32_t pid) override { return getParentModule()->getSubmodule("appsVectors", vmId)->getSubmodule("appModule", pid); }
      virtual CpuSchedulerRR *getScheduler(uint32_t vmId) override { return schedulers.at(vmId); }
      virtual void sendEvent(cMessage *msg) override;

      /**
       * Check if there are VMs running in the machine.
       * @return True if a VM is running in the machine. False otherwise.
       */
      void powerOn(bool active);
   };
};

#endif /*SIMCAN_EX_DC_HYPERVISOR_H__*/

#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    // Let the parent setup initialize first
    Hypervisor::initialize(stage);

    switch (stage)
    {
    case LOCAL:
        appsVector = getModuleByPath("^.apps");
        break;
    case NEAR:
    {
        /*
           The control tables were already initialized by the parent class

           We automatically reserve the only VM (vmId = 0) and set a reference
           to it as "local", so that the EdgeUser can directly request a deployment
         */
        uint32_t vmId = vmsControl.takeId();
        vmIdMap["local"] = vmId;

        VmControlBlock &vmControl = vmsControl[vmId];
        vmControl.globalId = &(vmIdMap.find("local")->first);
        vmControl.state = vmAccepted;
        break;
    }
    default:
        break;
    }
}

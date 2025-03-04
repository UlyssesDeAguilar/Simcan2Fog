#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    // Let the parent setup initialize first
    Hypervisor::initialize(stage);

    if (stage == LOCAL)
    {
        appsVector = getModuleByPath("^.apps");
        starterApps = par("apps");

        if (*starterApps)
        {
            UserAppBuilder builder;
            builder.createNewAppRequest("localApp", starterApps, "#0.0.0.0", "local", 0);
            auto vec = builder.finish("local", "local");
            sam = (*vec)[0];
            delete vec;
        }
    }
    else if (stage == NEAR)
    {
        /*
           The control tables were already initialized by the parent class

           We automatically reserve the only VM (vmId = 0) and set a reference
           to it as "local", so that the EdgeUser can directly request a deployment
         */
        uint32_t vmId = vmsControl.takeId();
        vmIdMap["local"] = vmId;

        VmControlBlock &vmControl = vmsControl[vmId];
        vmControl.globalId = "local";
        vmControl.state = vmRunning;
        vmControl.request = sam;
    }
    else if (stage == inet::INITSTAGE_LAST)
    {
        if (sam)
            handleAppRequest(sam);
    }
}

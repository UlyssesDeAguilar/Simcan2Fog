#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    switch (stage)
    {
    case INNER_STAGE:
        appsVector = getModuleByPath("^.apps");
        break;
    case LOCAL_STAGE:
        // Initialize the free PID table
    default:
        break;
    }
}

void EdgeHypervisor::handleAppRequest(SM_UserAPP *sm)
{
    // From the "user"/"manager" it's implied that the vmId should be here
    auto appRequest = sm;
    auto numApps = appRequest->getAppArraySize();
    // auto vmId = appRequest->getApp(0).vmId;
    // appRequest->getVmId();

    // If there are not sufficient spaces for the apps
    if (numApps > appsControl.getFreeIds())
    {
        appRequest->setResult(SM_APP_Res_Reject);
        appRequest->setIsResponse(true);
        sendResponseMessage(sm);
        return;
    }

    // If everthing is alright then launch the Apps
    osCore.launchApps(appRequest);
}
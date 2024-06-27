#include "OsCore.h"

#include "s2f/architecture/net/routing/RoutingInfo_m.h"
#include "s2f/os/hypervisors/Hypervisor.h"

using namespace hypervisor;

void OsCore::setUp(Hypervisor *h, DataManager *dm, HardwareManager *hm)
{
    this->hypervisor = h;
    this->dataManager = dm;
    this->hardwareManager = hm;
}

void OsCore::processSyscall(Syscall *request)
{
    // Get the app context
    auto appEntry = hypervisor->getAppControlBlock(request->getVmId(), request->getPid());

    // Select the appropiate handler or actions
    switch (request->getOpCode())
    {
    // Directly send the request to the CPU scheduler
    case SyscallCode::EXEC:
    {
        hypervisor->sendRequestMessage(request, hypervisor->schedulerGates.outBaseId + appEntry.vmId);
        break;
    }
    // Writing or reading from disk
    case SyscallCode::READ:
    case SyscallCode::WRITE:
    {
        hypervisor->sendRequestMessage(request, hypervisor->gate("toDisk"));
        break;
    }
    // Gracefully exit
    case SyscallCode::EXIT:
        handleAppTermination(appEntry, appFinishedOK);
        delete request;
        break;
    case SyscallCode::ABORT:
        handleAppTermination(appEntry, appFinishedError);
        delete request;
        break;
    default:
        hypervisor->error("Undefined system call operation code");
    }
}

void OsCore::launchApps(SM_UserAPP *request, uint32_t vmId, app_iterator begin, app_iterator end, const std::string &globalVmId)
{
    ApplicationBuilder::Context context;
    std::string userId(request->getUserId());

    // Init the userId context
    context.userId = &userId;

    for (; begin != end; ++begin)
    {
        // Retrieve the app
        auto &appInstance = *begin;

        // Query app
        auto schema = dataManager->searchApp(appInstance.appType);
        if (schema == nullptr)
            hypervisor->error("Error while querying the application type: %s", appInstance.appType.c_str());

        // Get new PID
        uint32_t newPid = hypervisor->takePid(vmId);

        // Initalize the control block
        AppControlBlock &control = hypervisor->getAppControlBlock(vmId, newPid);
        VmControlBlock &vmControl = hypervisor->vmsControl[vmId];
        control.deploymentIndex = request->getDeploymentIndex(begin);
        vmControl.request = request;

        // Load the context
        context.schema = schema;
        context.appId = &appInstance.serviceName;
        context.vmId = &globalVmId;

        // Locate slot and build
        cModule *parent = hypervisor->getApplicationModule(vmId, newPid);
        appBuilder.build(parent, context);

        // Update status
        appInstance.state = appRunning;
    }
}

void OsCore::handleAppTermination(AppControlBlock &app, tApplicationState exitStatus)
{
    VmControlBlock &vmControl = hypervisor->vmsControl[app.vmId];
    SM_UserAPP *userRequest = vmControl.request;

    auto deploymentIndex = app.deploymentIndex;

    // Release the pid
    hypervisor->releasePid(app.vmId, app.pid);

    // Topologically release the app
    cModule *parent = hypervisor->getApplicationModule(app.vmId, app.pid);
    appBuilder.deleteApp(parent);

    // Mark exit status and increase the count of finished apps.
    app.status = exitStatus;
    userRequest->changeStateByIndex(deploymentIndex, app.status);
    userRequest->increaseFinishedApps();

    // If all apps finished
    if (userRequest->allAppsFinished())
    {
        auto update = userRequest->dup();
        auto routingInfo = new RoutingInfo();

        // Internal routing
        routingInfo->setDestinationUrl(ServiceURL(DC_MANAGER_LOCAL_ADDR));
        routingInfo->setSourceUrl(ServiceURL(hypervisor->hardwareManager->getIp()));
        update->setControlInfo(routingInfo);

        // Global routing
        update->setDestinationTopic(userRequest->getReturnTopic());
        update->setResult(SM_APP_Res_Accept);
        update->setVmId(vmControl.globalId.c_str());
        update->setIsResponse(true);
        hypervisor->sendResponseMessage(update);

        // Bookkeeping, release the message
        vmControl.request = nullptr;
        delete userRequest;
    }

    // Reset the control block
    app.reset();
}

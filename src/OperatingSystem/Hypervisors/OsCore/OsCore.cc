#include "OsCore.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"

using namespace hypervisor;

void OsCore::setUp(Hypervisor *h, DataManager *dm, HardwareManager *hm)
{
    this->hypervisor = h;
    this->dataManager = dm;
    this->hardwareManager = hm;
}

void OsCore::processSyscall(SM_Syscall *request)
{
    // Get the app context
    auto appEntry = hypervisor->getAppControlBlock(request->getVmId(), request->getPid());

    // Register the incoming request and get the context
    const CallContext& callContext = request->getContext();

    // Select the appropiate handler or actions
    switch (callContext.opCode)
    {
    // Directly send the request to the CPU scheduler
    case Syscall::EXEC:
    {
        auto label = new AppIdLabel();
        label->setPid(appEntry.pid);
        label->setVmId(appEntry.vmId);
        callContext.data.cpuRequest->setControlInfo(label);
        callContext.data.cpuRequest->setContextPointer(request);
        hypervisor->sendRequestMessage(callContext.data.cpuRequest, hypervisor->schedulerGates.outBaseId + appEntry.vmId);
        break;
    }
    // Writing or reading from disk
    case Syscall::READ:
    case Syscall::WRITE:
    {
        auto label = new AppIdLabel();
        label->setPid(appEntry.pid);
        label->setVmId(appEntry.vmId);
        callContext.data.cpuRequest->setControlInfo(label);
        hypervisor->sendRequestMessage(request, hypervisor->gate("toDisk"));
        break;
    }
    // TODO: Networking
    case Syscall::OPEN_CLI:
        break;
    case Syscall::OPEN_SERV:
        break;
    case Syscall::SEND:
        break;
    case Syscall::RECV:
        break;
    // Gracefully exit
    case Syscall::EXIT:
        handleAppTermination(appEntry, false);
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
        control.deploymentIndex = request->getDeploymentIndex(begin);
        control.request = request;

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

void OsCore::handleAppTermination(AppControlBlock &app, bool force)
{
    auto userRequest = app.request;
    auto deploymentIndex = app.deploymentIndex;

    // Release the pid
    hypervisor->releasePid(app.vmId, app.pid);

    // Topologically release the app
    cModule *parent = hypervisor->getApplicationModule(app.vmId, app.pid);
    appBuilder.deleteApp(parent);

    // Mark exit status and increase the count of finished apps.
    if (force)
        app.status = tApplicationState::appFinishedTimeout;
    else
        app.status = tApplicationState::appFinishedOK;

    userRequest->changeStateByIndex(deploymentIndex, app.status);
    userRequest->increaseFinishedApps();

    // If all apps finished
    if (userRequest->allAppsFinished())
        hypervisor->sendResponseMessage(userRequest);

    // Reset the control block
    app.reset();
}
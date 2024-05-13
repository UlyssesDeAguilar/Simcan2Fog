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

    // TODO: Sanity checks -- Zombie requests

    // Register the incoming request and get the context
    appEntry.lastRequest = request;
    auto callContext = request->getContext();

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
        hypervisor->sendRequestMessage(callContext.data.cpuRequest, hypervisor->schedulerGates.outBaseId + appEntry.vmId);
        break;
    }
    // Writing or reading from disk is pretty similar
    case Syscall::READ:
    {
        auto readBytes = callContext.data.bufferSize;
        simtime_t eta = readBytes / hardwareManager->getDiskSpecs().readBandwidth;
        auto event = new cMessage("IO Complete", AutoEvent::IO_DELAY);
        event->setContextPointer(&appEntry);
        hypervisor->scheduleAt(eta, event);
        break;
    }
    case Syscall::WRITE:
    {
        auto writeBytes = callContext.data.bufferSize;
        simtime_t eta = writeBytes / hardwareManager->getDiskSpecs().writeBandwidth;
        auto event = new cMessage("IO Complete", AutoEvent::IO_DELAY);
        event->setContextPointer(&appEntry);
        hypervisor->scheduleAt(eta, event);
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

void OsCore::launchApps(SM_UserAPP *request, uint32_t vmId, app_iterator begin, app_iterator end)
{
    ApplicationBuilder::Context context;
    std::string userId(request->getUserID());

    // Init the userId context
    context.userId = &userId;

    for (; begin != end; ++begin)
    {
        // Retrieve the app
        auto &appInstance = *begin;

        // Query app
        auto schema = dataManager->searchApp(appInstance.strAppType);
        if (schema == nullptr)
            hypervisor->error("Error while querying the application type: %s", appInstance.strAppType.c_str());

        // Get new PID
        uint32_t newPid = hypervisor->takePid(vmId);

        // Initalize the control block
        AppControlBlock &control = hypervisor->getAppControlBlock(vmId, newPid);
        control.deploymentIndex = request->getDeploymentIndex(begin);
        control.request = request;

        // Load the context
        context.schema = schema;
        context.appId = &appInstance.strApp;
        context.vmId = &appInstance.vmId;

        // Locate slot and build
        cModule *parent = hypervisor->getApplicationModule(vmId, newPid);
        appBuilder.build(parent, context);

        // Update status
        appInstance.eState = appRunning;
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

void OsCore::handleIOFinish(AppControlBlock &app)
{
    // Get the original request
    auto request = app.lastRequest;

    // Set all OK
    request->setResult(SC_OK);
    request->setIsResponse(true);

    // Send back to app and clear the request from control block
    hypervisor->sendResponseMessage(request);
    app.lastRequest = nullptr;
}
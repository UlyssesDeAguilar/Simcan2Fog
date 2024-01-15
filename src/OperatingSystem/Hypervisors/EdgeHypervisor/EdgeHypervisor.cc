#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize()
{
    // Get the parameters
    maxApps = par("maxApps");
    runningApps = 0;

    // TODO: Consider moving this functionality to the HW manager
    // Get the parameters of the parent module
    auto parent = getParentModule();
    hwSpecs.totalCores = parent->par("numCpuCores");
    hwSpecs.totalMemory = parent->par("memorySize");
    hwSpecs.disk.total = parent->par("diskSize");
    hwSpecs.disk.readBandwidth = parent->par("diskReadBandwidth");
    hwSpecs.disk.writeBandwidth = parent->par("diskWriteBandwidth");

    // Initialize the free PID table
    freePids.reserve(maxApps);
    for (int i = 0; i < maxApps; i++)
        freePids.push_back(i);

    // Create the control table
    appsControl = new AppControlBlock[maxApps];

    // Initialize the App Control Entries
    // The edge hypervisor doesn't contain VMs per se so the vm id is 0
    for (int i = 0; i < maxApps; i++)
        appsControl->initialize(i, 0);

    // Get the direct pointer to the HardwareManager and the App Vector module
    hardwareManager = check_and_cast<HardwareManager *>(getModuleByPath("^.hwm"));
    appsVector = getModuleByPath("^.apps");
}

void EdgeHypervisor::finish()
{
    // Release and mark as null
    delete[] appsControl;
    appsControl = nullptr;
}

cGate *EdgeHypervisor::getOutGate(cMessage *msg) {}

void EdgeHypervisor::processSelfMessage(cMessage *msg)
{
    auto appEntry = *(AppControlBlock *)msg->getContextPointer();

    switch (msg->getKind())
    {
    case AutoEvent::IO_DELAY:
        handleIOFinish(appEntry);
        break;
    case AutoEvent::APP_TIMEOUT:
        handleAppTermination(appEntry, true);
        break;
    default:
        delete msg;
        error("Unkown auto event of kind: %d", msg->getKind());
        break;
    }

    delete msg;
}

void EdgeHypervisor::processRequestMessage(SIMCAN_Message *msg)
{
    // It can be either
    // 1 - An app launch request
    // 2 - A CPU execution request
    // 3 - A network request
    // 4 - An app finished it's execution

    switch (msg->getOperation())
    {
    case SM_APP_Req:
    {
        auto appRequest = (SM_UserAPP *)msg;
        auto numApps = appRequest->getAppArraySize();

        // If there are not sufficient spaces for the apps
        if (numApps > freePids.size())
        {
            appRequest->setResult(SM_APP_Res_Reject);
            appRequest->setIsResponse(true);
            sendResponseMessage(msg);
            return;
        }

        // If everthing is alright then launch the Apps
        launchApps(appRequest);
        break;
    }
    case SM_Syscall_Req:
        processSyscall((SM_Syscall *)msg);
        break;
    // Add the network packages !
    default:
        break;
    }
}

void EdgeHypervisor::processSyscall(SM_Syscall *request)
{
    // Get the app context (this could be more general!)
    auto appEntry = appsControl[request->getPid()];

    // TODO: Sanity checks

    // Register the incoming request
    appEntry.lastRequest = request;

    // Get the system call context
    auto callContext = request->getContext();

    // Select the appropiate handler or actions
    switch (callContext.opCode)
    {
    // Directly send the request to the CPU scheduler
    case Syscall::EXEC:{
        auto label = new AppIdLabel();
        label->setPid(appEntry.pid);
        label->setVmId(appEntry.vmId);
        callContext.data.cpuRequest->setControlInfo(label);
        sendRequestMessage(callContext.data.cpuRequest, gate("toCpuScheduler"));
        break;
    }
    // Writing or reading from disk is pretty similar
    case Syscall::READ:{
        auto readBytes = callContext.data.bufferSize;
        simtime_t eta = readBytes / hwSpecs.disk.readBandwidth;
        auto event = new cMessage("IO Complete",AutoEvent::IO_DELAY);
        event->setContextPointer(&appEntry);
        scheduleAt(eta,event);
        break;
    }
    case Syscall::WRITE:{
        auto writeBytes = callContext.data.bufferSize;
        simtime_t eta = writeBytes / hwSpecs.disk.writeBandwidth;
        auto event = new cMessage("IO Complete",AutoEvent::IO_DELAY);
        event->setContextPointer(&appEntry);
        scheduleAt(eta,event);
        break;
    }
    // TODO: Networking
    case Syscall::SEND:
        break;
    case Syscall::REGISTER_SERVICE:
        break;
    // Gracefully exit
    case Syscall::EXIT:
        handleAppTermination(appEntry, false);
        break;
    default:
        error("Undefined system call operation code");
    }
}

void EdgeHypervisor::processResponseMessage(SIMCAN_Message *msg)
{
    // Mostly it will be:
    // 1 - CPU status update (including finish)
    // 2 - Network response (should check the port)
    switch (msg->getOperation())
    {
    case SM_ExecCpu:{
        auto appId = check_and_cast<AppIdLabel*>(msg->getControlInfo());
        auto cpuRequest = check_and_cast<SM_CPU_Message*>(msg);

        // If the batch is complete -> notify the app
        if (cpuRequest->isCompleted())
        {
            // appId->getVmId(); Useful when we will consider also managing VMs
            auto appEntry = appsControl[appId->getPid()];
            appEntry.lastRequest = nullptr;
            sendResponseMessage(msg);
        }else
        {
            // TODO: Update or emit statistics ?
            delete cpuRequest;
        }
        break;
    }
    default:
        break;
    }
}

void EdgeHypervisor::launchApps(SM_UserAPP *request)
{
    for (int i = 0; i < request->getAppArraySize(); i++)
    {
        // Retrieve the app
        auto appInstance = request->getApp(i);

        // Initalize the control block
        uint32_t newPid = freePids.back();
        freePids.pop_back();

        // TODO: Construct the app module -- Lookups / Typing
        cModule app;
    }
}

void EdgeHypervisor::handleAppTermination(AppControlBlock &app, bool force) {}

void EdgeHypervisor::handleIOFinish(AppControlBlock &app)
{
    // Get the original request
    auto request = app.lastRequest;
    
    // Set all OK
    request->setResult(SC_OK);
    request->setIsResponse(true);

    // Send back to app and clear the request from control block
    sendResponseMessage(request);
    app.lastRequest = nullptr;
}

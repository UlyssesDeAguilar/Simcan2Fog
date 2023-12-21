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
    auto appEntry = (AppControlBlock *)msg->getContextPointer();

    switch (msg->getKind())
    {
    case AutoEvent::IO_DELAY:
        handleIOAppRequest(appEntry->pid, true);
        break;
    case AutoEvent::APP_TIMEOUT:
        handleAppTermination(appEntry->pid, true);
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
    case SM_APP_Req:{
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
        processSyscall((SM_Syscall *) msg);
        break;
    // Add the network packages !
    default:
        break;
    }
}

void EdgeHypervisor::processSyscall(SM_Syscall *request)
{
    // TODO: Check if app instance is currently running !
    // Get the app context
    auto appEntry = appsControl[request->getPid()];
    appEntry.lastRequest = request;
    auto callContext = request->getContext();
    switch (callContext.opCode){
        case Syscall::EXEC:
            break;
        case Syscall::READ:
        case Syscall::WRITE:
            break;
        case Syscall::SEND_NETWORK:
            break;
        case Syscall::BIND_AND_LISTEN:
            break;
        case Syscall::EXIT:
            break;
        default:
            error("Undefined system call operation code");
    }
}

void EdgeHypervisor::processResponseMessage(SIMCAN_Message *msg)
{
    // Mostly it will be:
    // 1 - CPU timeout
    // 2 - Network response (should check the port)
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

void EdgeHypervisor::handleAppTermination(uint32_t pid, bool force){}

void EdgeHypervisor::handleIOAppRequest(uint32_t pid, bool completed)
{
    if (completed)
    {
        // Recover app context
        auto appEntry = appsControl[pid];

        // Set all OK
        appEntry.lastRequest->setResult(SC_OK);
        appEntry.lastRequest->setIsResponse(true);

        // Send back to app
        sendResponseMessage(appEntry.lastRequest);
    }
    else
    {
        // TODO: Select Read or write accordingly
        double size = 500;
        double bandwidth = hwSpecs.disk.readBandwidth;

        // Calculate delay and set timer to emulate IO operations
        simtime_t eta = size / bandwidth;
        auto completionEvent = new cMessage("IO complete", AutoEvent::IO_DELAY);
        scheduleAt(eta, completionEvent);
    }
}

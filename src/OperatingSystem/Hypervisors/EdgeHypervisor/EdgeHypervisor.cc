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

    // Initialize the free PID table
    freePids.reserve(maxApps);
    for (int i = 0; i < maxApps; i++)
        freePids.push_back(i);

    // Allocate the control table to contain app blocks (and fit)
    appsControl.init(AppControlBlock::initialize);

    // Locate topologically the helper modules
    DataManager *dataManager = check_and_cast<DataManager *>(getModuleByPath("simData.manager"));
    hardwareManager = check_and_cast<HardwareManager *>(getModuleByPath("^.hwm"));

    // Initialize the OsCore
    osCore.setUp(this, dataManager, hardwareManager);

    // Get the direct pointer to the HardwareManager and the App Vector module
    appsVector = getModuleByPath("^.apps");
}

void EdgeHypervisor::finish()
{
    // Now it's empty as we use a std::vector!
}

cGate *EdgeHypervisor::getOutGate(cMessage *msg)
{
    auto sm = check_and_cast<SIMCAN_Message *>(msg);

    switch (sm->getOperation())
    {
    case SM_Syscall_Req:
    {
        auto syscall = check_and_cast<SM_Syscall *>(msg);

        // This allows the app hub to return the result
        sm->setNextModuleIndex(syscall->getPid());

        // return gate() asssociated with syscall->getVmId() )
        // namely gate("toApps", getVmId())
        return gate("toApps");
        break;
    }
    default:
        return nullptr;
    }
}

void EdgeHypervisor::processSelfMessage(cMessage *msg)
{
    auto appEntry = *(AppControlBlock *)msg->getContextPointer();

    switch (msg->getKind())
    {
    case AutoEvent::IO_DELAY:
        osCore.handleIOFinish(appEntry);
        break;
    default:
        error("Unkown auto event of kind: %d", msg->getKind());
        delete msg;
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
        osCore.launchApps(appRequest);
        break;
    }
    case SM_Syscall_Req:
        osCore.processSyscall(check_and_cast<SM_Syscall *>(msg));
        break;
    // Add the network packages !
    default:
        break;
    }
}

void EdgeHypervisor::processResponseMessage(SIMCAN_Message *msg)
{
    // Mostly it will be:
    // 1 - CPU status update (including finish)
    // 2 - Network response (should check the port)
    switch (msg->getOperation())
    {
    case SM_ExecCpu:
    {
        auto appId = check_and_cast<AppIdLabel *>(msg->getControlInfo());
        auto cpuRequest = check_and_cast<SM_CPU_Message *>(msg);

        // If the batch is complete -> notify the app
        if (cpuRequest->isCompleted())
        {
            // appId->getVmId(); Useful when we will consider also managing VMs
            auto appEntry = appsControl[appId->getPid()];
            appEntry.lastRequest = nullptr;
            sendResponseMessage(msg);
        }
        else
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

uint32_t EdgeHypervisor::newPid(int vmId)
{
    int newPid = freePids.back();
    freePids.pop_back();
    return newPid;
}
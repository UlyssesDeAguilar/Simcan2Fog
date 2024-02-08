#include "Hypervisor.h"
using namespace hypervisor;

void Hypervisor::initialize(int stage)
{
    switch (stage)
    {
    case INNER_STAGE:
    {
        // Locate topologically the helper modules
        DataManager *dataManager = check_and_cast<DataManager *>(getModuleByPath("simData.manager"));
        hardwareManager = locateHardwareManager();

        // Initialize the OsCore
        osCore.setUp(this, dataManager, hardwareManager);
        break;
    }
    case LOCAL_STAGE:
    {
        // Get from hardware manager the specs of the node
        auto maxUsers = hardwareManager->getTotalResources().users;
        auto maxVms = hardwareManager->getTotalResources().vms;
        maxAppsPerVm = par("maxApps");

        // Create and allocate control tables
        vmsControl.init(maxVms, &VmControlBlock::initialize);
        for (auto &vm : vmsControl)
        {
            vm.second.apps.init(maxAppsPerVm, &AppControlBlock::initialize);
        }
    }
    default:
        break;
    }
}

void Hypervisor::finish()
{
    // Now it's empty as we use a std::vector!
}

cGate *Hypervisor::getOutGate(cMessage *msg)
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

void Hypervisor::processSelfMessage(cMessage *msg)
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

void Hypervisor::processRequestMessage(SIMCAN_Message *msg)
{
    // It can be either
    // 1 - An app launch request   -- From ethernet
    // 2 - A CPU execution request -- From apps
    // 3 - A network request       -- From apps
    // 4 - An app finished it's execution -- From apps/¿user?

    switch (msg->getOperation())
    {
    case SM_APP_Req:
    {
        handleAppRequest(reinterpret_cast<SM_UserAPP *>(msg));
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

void Hypervisor::processResponseMessage(SIMCAN_Message *msg)
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
            auto appEntry = getAppControlBlock(appId->getVmId(), appId->getPid());
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

void Hypervisor::handleAppRequest(SM_UserAPP *sm)
{
    // From the "user"/"manager" it's implied that the vmId should be here
    auto appRequest = sm;

    // For each vm in the request
    for (auto &vmApps : *sm)
    {
        // Attempt recovering the vm "reservation"
        uint32_t vmId = resolveGlobalVmId(vmApps.element);

        // If found and there's enough space then start working
        if (vmId != UINT32_MAX && vmApps.size() <= vmsControl[vmId].apps.getFreeIds())
        {
            auto control = vmsControl.at(vmId);
            osCore.launchApps(sm, vmId, vmApps.begin(), vmApps.end());
        }
        else
        {
            sm->abortAllApps(vmApps.element);
        }
    }

    // Send update -> Deployment status?
}
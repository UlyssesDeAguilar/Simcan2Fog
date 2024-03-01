#include "Hypervisor.h"
using namespace hypervisor;

void Hypervisor::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
        // Retrieve info from input/output gates
        appGates.inBaseId = gateBaseId("fromApps");
        appGates.outBaseId = gateBaseId("toApps");
        schedulerGates.inBaseId = gateBaseId("fromCpuScheduler");
        schedulerGates.outBaseId = gateBaseId("toCpuScheduler");
        networkGates.inBaseId = gateBaseId("networkComm$i");
        networkGates.outBaseId = gateBaseId("networkComm$o");

        // Locate topologically the helper modules
        dataManager = check_and_cast<DataManager *>(getModuleByPath("simData.manager"));
        hardwareManager = locateHardwareManager();

        // Initialize the OsCore
        osCore.setUp(this, dataManager, hardwareManager);
        break;
    }
    case NEAR:
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

    cSIMCAN_Core::initialize(stage);
}

void Hypervisor::finish()
{
    // Now it's empty as we use a std::vector!
}

cGate *Hypervisor::getOutGate(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    int baseIndex = arrivalGate->getBaseId();

    // If it came from the network
    if (networkGates.inBaseId == baseIndex)
        return gate(networkGates.outBaseId);
    else if (appGates.inBaseId == baseIndex)
    {
        auto syscall = check_and_cast<SM_Syscall *>(msg);
        int arrivalIndex = arrivalGate->getIndex();

        // This allows the app hub to return the result
        syscall->setNextModuleIndex(syscall->getPid());

        return gate(appGates.outBaseId + arrivalIndex);
    }

    // Shouldn't accept the request
    return nullptr;
}

void Hypervisor::processSelfMessage(cMessage *msg)
{

    switch (msg->getKind())
    {
    case AutoEvent::IO_DELAY:
    {
        auto &appEntry = *reinterpret_cast<AppControlBlock*>(msg->getContextPointer());
        osCore.handleIOFinish(appEntry);
        break;
    }
    case AutoEvent::VM_TIMEOUT:
    {
        auto &vmEntry = *reinterpret_cast<VmControlBlock*>(msg->getContextPointer());
        handleVmTimeout(vmEntry);
    }
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
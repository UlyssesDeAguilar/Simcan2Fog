#include "Hypervisor.h"

using namespace hypervisor;
using namespace networkio;

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
        // auto maxUsers = hardwareManager->getTotalResources().users;
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
    for (auto &entry : vmsControl)
    {
        if (!entry.first)
        {
            VmControlBlock &control = entry.second;
            cancelAndDelete(control.timeOut);
            if (control.request)
                delete control.request;
        }
    }

    // Reset the control tables
    vmsControl.clear();
}

void Hypervisor::handleMessage(cMessage *msg)
{
    auto event = dynamic_cast<IncomingEvent *>(msg);
    if (event)
        handleIncomingEvent(event);
    else
        cSIMCAN_Core::handleMessage(msg);
}

cGate *Hypervisor::getOutGate(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();

    // This bit of selection is due to trace handling when we send a request
    if (arrivalGate == nullptr)
        return nullptr;

    int baseIndex = arrivalGate->getBaseId();

    // If it came from the network
    if (networkGates.inBaseId == baseIndex)
        return gate(networkGates.outBaseId);
    else if (appGates.inBaseId == baseIndex)
    {
        auto syscall = check_and_cast<Syscall *>(msg);
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
    case AutoEvent::VM_TIMEOUT:
    {
        auto &vmEntry = *reinterpret_cast<VmControlBlock *>(msg->getContextPointer());
        handleVmTimeout(vmEntry);
        break;
    }
    default:
        error("Unkown auto event of kind: %d", msg->getKind());
        delete msg;
        break;
    }
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
    case SM_AbortCpu:
    case SM_ExecCpu:
    case SM_Syscall_Req:
    {
        auto sysCall = check_and_cast<Syscall *>(msg);
        VmControlBlock &vmControl = vmsControl.at(sysCall->getVmId());

        if (vmControl.state == vmSuspended)
            vmControl.callBuffer.push_back(sysCall);
        else
            osCore.processSyscall(sysCall);
        break;
    }
    // Add the network packages !
    default:
        delete msg;
        error("Unkown syscall!");
        break;
    }
}

void Hypervisor::processResponseMessage(SIMCAN_Message *msg)
{
    // Mostly it will be:
    // 1 - CPU status update (including finish)
    // 2 - Disk finished IO

    auto response = check_and_cast<Syscall *>(msg);

    // If it is an update from the scheduler
    if (response->getOperation() == SM_ExecCpu)
    {
        auto cpuRequest = check_and_cast<SM_CPU_Message *>(msg);
        if (!cpuRequest->isCompleted())
        {
            delete msg;
            return;
        }
    }

    response->setIsResponse(true);
    VmControlBlock &vmControl = vmsControl.at(response->getVmId());

    // Assuming disk io finished
    if (vmControl.state == vmSuspended)
        vmControl.callBuffer.push_back(response);
    else
        sendResponseMessage(response);
}

void Hypervisor::handleAppRequest(SM_UserAPP *sm)
{
    Enter_Method_Silent();

    // From the "user"/"manager" it's implied that the vmId should be here
    // For each vm in the request
    for (auto &vmApps : *sm)
    {
        // Attempt recovering the vm "reservation"
        uint32_t vmId = resolveGlobalVmId(vmApps.element);

        // If found and there's enough space then start working
        if (vmId != UINT32_MAX && vmApps.size() <= vmsControl[vmId].apps.getFreeIds())
        {
            VmControlBlock &control = vmsControl.at(vmId);
            control.request = sm;
            osCore.launchApps(sm, vmId, vmApps.begin(), vmApps.end(), vmApps.element);
        }
        else
        {
            sm->abortAllApps(vmApps.element);
        }
    }

    // Send update -> Deployment status?
}

void Hypervisor::handleIncomingEvent(IncomingEvent *event)
{
    auto pid = event->getPid();
    auto vmId = event->getVmId();

    Syscall *result = nullptr;

    switch (event->getType())
    {
    case RESOLVER_NOERROR:
    {
        auto sys = new ResolverSyscall();
        sys->setOpCode(RESOLVE);
        sys->setResult(OK);
        sys->setResolvedIp(event->getResolvedIp());
        result = sys;
        break;
    }
    case RESOLVER_NXDOMAIN:
    {
        auto sys = new ResolverSyscall();
        sys->setOpCode(RESOLVE);
        sys->setResult(ERROR);
        sys->setDomainName(event->getServiceName());
        result = sys;
        break;
    }
    case SOCKET_ESTABLISHED:
    {
        auto sys = new SocketIoSyscall();
        sys->setOpCode(OPEN_CLI);
        sys->setKind(event->getKind());
        sys->setResult(OK);
        sys->setSocketFd(event->getSocketId());
        result = sys;
        break;
    }
    case SOCKET_DATA_ARRIVED:
    {
        auto sys = new SocketIoSyscall();
        sys->setOpCode(RECV);
        sys->setResult(OK);
        sys->setSocketFd(event->getSocketId());
        sys->setPayload(event->getPayload());
        result = sys;
        break;
    }
    case SOCKET_FAILURE:
    case SOCKET_PEER_CLOSED:
    {
        auto sys = new SocketIoSyscall();
        sys->setOpCode(RECV);
        sys->setResult(ERROR);
        sys->setSocketFd(event->getSocketId());
        result = sys;
        break;
    }
    default:
        error("Could not interpret network event with code %d", event->getType());
        break;
    }

    result->setVmId(vmId);
    result->setPid(pid);
    result->setNextModuleIndex(pid);
    sendRequestMessage(result, appGates.outBaseId + vmId);
    delete event;
}

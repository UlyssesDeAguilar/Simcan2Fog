#include "OsCore.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"
#include "Architecture/Network/RoutingInfo/RoutingInfo_m.h"
#include "Architecture/Network/Stack/StackServiceType.h"

using namespace hypervisor;
using namespace networkio;

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
    case SyscallCode::RESOLVE:
    {
        auto resolve = reinterpret_cast<ResolverSyscall *>(request);
        auto command = new CommandEvent();
        fillNetworkData(request, command);
        command->setServiceName(resolve->getDomainName());
        hypervisor->send(command, hypervisor->networkGates.outBaseId);
        delete request;
        break;
    }
    case SyscallCode::OPEN_SERV:
    {
        auto socketIo = reinterpret_cast<SocketIoSyscall *>(request);
        auto command = new CommandEvent();
        fillNetworkData(request, command);

        if (socketIo->getMode() == UDP)
            command->setKind(StackServiceType::UDP_IO);
        else
        {
            command->setKind(StackServiceType::HTTP_PROXY);
            command->setServiceName(socketIo->getServiceName());
        }

        command->setCommand(SOCKET_OPEN);
        command->setTargetPort(socketIo->getTargetPort());
        hypervisor->send(command, hypervisor->networkGates.outBaseId);
        delete request;
        break;
    }
    case SyscallCode::OPEN_CLI:
    {
        auto socketIo = reinterpret_cast<SocketIoSyscall *>(request);
        auto command = new CommandEvent();
        fillNetworkData(request, command);

        if (socketIo->getMode() == UDP)
            command->setKind(StackServiceType::UDP_IO);
        else
            command->setKind(StackServiceType::HTTP_CLIENT);

        command->setTargetPort(socketIo->getTargetPort());
        command->setTargetIp(socketIo->getTargetIp());
        command->setCommand(SOCKET_OPEN);
        hypervisor->send(command, hypervisor->networkGates.outBaseId);
        delete request;
        break;
    }
    case SyscallCode::SEND:
    {
        auto socketIo = reinterpret_cast<SocketIoSyscall *>(request);
        auto command = new CommandEvent();
        fillNetworkData(request, command);

        switch (socketIo->getMode())
        {
        case UDP:
        {
            command->setKind(StackServiceType::UDP_IO);
            command->setTargetPort(socketIo->getTargetPort());
            command->setTargetIp(socketIo->getTargetIp());
            break;
        }
        case TCP_SERVER:
            command->setKind(StackServiceType::HTTP_PROXY);
            break;
        case TCP_CLIENT:
            command->setKind(StackServiceType::HTTP_CLIENT);
            break;
        default:
            break;
        }

        command->setSocketId(socketIo->getSocketFd());
        command->setCommand(SOCKET_SEND);

        hypervisor->send(command, hypervisor->networkGates.outBaseId);
        delete request;
        break;
    }
    case SyscallCode::CLOSE:
    {
        auto socketIo = reinterpret_cast<SocketIoSyscall *>(request);
        auto command = new CommandEvent();
        fillNetworkData(request, command);

        switch (socketIo->getMode())
        {
        case UDP:
            command->setKind(StackServiceType::UDP_IO);
            break;
        case TCP_SERVER:
            command->setKind(StackServiceType::HTTP_PROXY);
            break;
        case TCP_CLIENT:
            command->setKind(StackServiceType::HTTP_CLIENT);
            break;
        default:
            break;
        }

        command->setSocketId(socketIo->getSocketFd());
        command->setCommand(SOCKET_CLOSE);
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

void OsCore::fillNetworkData(Syscall *sys, CommandEvent *e)
{
    e->setPid(sys->getPid());
    e->setVmId(sys->getVmId());
    e->setIp(hardwareManager->getIp());

    // This section is for the dc environement
    auto routingInfo = new RoutingInfo();
    routingInfo->setDestinationUrl(DC_NETWORK_STACK);
    e->setControlInfo(routingInfo);
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
        update->setVmId(vmControl.globalId->c_str());
        update->setIsResponse(true);
        hypervisor->sendResponseMessage(update);

        // Bookkeeping, release the message
        vmControl.request = nullptr;
        delete userRequest;
    }

    // Reset the control block
    app.reset();
}

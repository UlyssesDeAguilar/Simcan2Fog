#include "Hypervisor.h"

using namespace omnetpp;
using namespace s2f::os;

Register_Abstract_Class(Hypervisor);

void Hypervisor::initialize()
{
    // Retrieve info from input/output gates
    appGates.inBaseId = gateBaseId("fromApps");
    appGates.outBaseId = gateBaseId("toApps");

    schedulerGates.inBaseId = gateBaseId("fromCpuScheduler");
    schedulerGates.outBaseId = gateBaseId("toCpuScheduler");

    networkGates.inBaseId = gateBaseId("fromNetwork");
    networkGates.outBaseId = gateBaseId("toNetwork");

    // Locate topologically the helper modules
    dataManager = check_and_cast<DataManager *>(getModuleByPath(par("dataManagerPath")));
    hardwareManager = check_and_cast<HardwareManager *>(getModuleByPath(par("hardwareManagerPath")));
    controlTable = check_and_cast<VmControlTable *>(getModuleByPath(par("controlTablePath")));
    maxAppsPerVm = par("maxAppsPerVm");
}

void Hypervisor::finish() {}

void Hypervisor::handleMessage(cMessage *msg)
{
    msg = L2Protocol::tryDecapsulate(msg);
    auto sm = check_and_cast<SIMCAN_Message *>(msg);

    if (messageIsCommand(sm))
        processCommand(sm);
    else if (sm->isResponse())
        processSyscallEnd(check_and_cast<Syscall *>(sm));
    else
        processSyscallStart(check_and_cast<Syscall *>(sm));
}

void Hypervisor::processCommand(SIMCAN_Message *msg)
{
    if (msg->getOperation() == SM_APP_Req)
        handleAppRequest(check_and_cast<SM_UserAPP *>(msg));
    else if (msg->getOperation() == SM_VM_Req)
        handleVmRequest(check_and_cast<SM_UserVM *>(msg));
    else if (msg->getOperation() == SM_ExtensionOffer)
        handleVmExtension(check_and_cast<SM_VmExtend *>(msg));
    else if (msg->getOperation() == SM_App_Set)
        handleAppSettings(check_and_cast<SM_AppSettings *>(msg));
    else
        error("Unexpected command %s with code %d", msg->getClassName() ,msg->getOperation());
}

void Hypervisor::handleAppRequest(SM_UserAPP *sm)
{
    ApplicationBuilder::Context context;

    // Init context and resolve the VM mapping
    context.userId = sm->getUserId();
    context.vmId = sm->getVmId();
    int localVmId = controlTable->getLocalVmId(sm->getVmId());
    controlTable->associateDeploymentToVm(localVmId, sm);

    for (int i = 0, size = sm->getAppArraySize(); i < size; i++)
    {
        // Retrieve the app
        const AppRequest &appInstance = sm->getApp(i);

        // Query app
        const Application *schema = dataManager->searchApp(appInstance.getType());
        if (schema == nullptr)
            error("Error while querying the application type: %s", appInstance.getType());

        // Get new PID
        int newPid = controlTable->allocateApp(localVmId, i);

        // Load the context and build
        context.schema = schema;
        context.appId = appInstance.getName();
        cModule *parent = getApplicationModule(localVmId, newPid);
        appBuilder.build(parent, context);
    }
}

void Hypervisor::handleVmRequest(SM_UserVM *request)
{
    if (!par("isVirtualHardware"))
        error("Only virtual hardware is supported for vm requests!");

    for (int i = 0, size = request->getVmArraySize(); i < size; i++)
    {
        unsigned int *cpuCoreIndex;
        VM_Request &vm = request->getVmForUpdate(i);
        const VirtualMachine *vmTemplate = dataManager->searchVm(vm.vmType);

        // Attempt allocating needed space
        if (!hardwareManager->tryAllocateResources(vmTemplate, &cpuCoreIndex))
            error("Unexpected failure on allocating vm resources");

        int vmId = controlTable->allocateVm(vmTemplate, request->getUserId());
        controlTable->createMapping(vm.vmId.c_str(), vmId);

        // Setup the scheduler
        CpuSchedulerRR *pVmScheduler = getScheduler(vmId);
        pVmScheduler->setManagedCpuCores(vmTemplate->getNumCores());
        pVmScheduler->setCpuCoreIndex(cpuCoreIndex);
        pVmScheduler->setIsRunning(true);
    }

    delete request;
}

void Hypervisor::handleVmExtension(SM_VmExtend *sm)
{
    int vmId = controlTable->getLocalVmId(sm->getVmId());
    if (sm->getAccepted())
    {
        cQueue &callBuffer = controlTable->resumeVm(vmId);
        while (callBuffer.isEmpty())
        {
            auto syscall = check_and_cast<Syscall *>(callBuffer.pop());
            if (syscall->isResponse())
                processSyscallEnd(syscall);
            else
                processSyscallStart(syscall);
        }
        // TODO Restart the counter
    }
    else
        deallocateVmResources(vmId);
    
    delete sm;
}

void Hypervisor::processSyscallStart(Syscall *request)
{
    int vmId = request->getVmId();
    if (controlTable->vmIsSuspended(vmId))
    {
        controlTable->bufferCall(vmId, request);
        return;
    }

    if (request->getOpCode() == SyscallCode::EXEC)
        send(request, schedulerGates.outBaseId + vmId);
    else if (request->getOpCode() == SyscallCode::READ || request->getOpCode() == SyscallCode::WRITE)
        send(request, gate("toDisk"));
    else if (request->getOpCode() == SyscallCode::EXIT || request->getOpCode() == SyscallCode::ABORT)
    {
        handleAppTermination(request->getPid(), request->getVmId(), request->getOpCode() == SyscallCode::EXIT ? appFinishedOK : appFinishedError);
        delete request;
    }
    else
        error("Undefined system call operation code");
}

void Hypervisor::processSyscallEnd(Syscall *response)
{
    // If it is an update from the scheduler
    if (response->getOperation() == SM_ExecCpu)
    {
        auto cpuRequest = check_and_cast<SM_CPU_Message *>(response);
        if (!cpuRequest->isCompleted())
        {
            delete response;
            return;
        }
    }

    response->setIsResponse(true);

    // Assuming disk io finished
    sendOrBufferToApp(response);
}

void Hypervisor::handleAppSettings(SM_AppSettings *settings) 
{
	EV_DEBUG << "Refresh settings for app\n";
    int vmId = controlTable->getLocalVmId(settings->getVmId());
    int pid = controlTable->getPidFromServiceName(vmId, settings->getAppName());
    cModule *app = getApplicationModule(vmId, pid)->getSubmodule("app");
    
    for (int i = 0, size = settings->getParametersArraySize(); i < size; i++)
    {
        auto &set = settings->getParameters(i);
        cPar &param = app->par(set.name.c_str());
        if (param.getType() == cPar::STRING)
            param.setStringValue(set.value.c_str());
        else
            param.parse(set.value.c_str());
    }

    delete settings;
}

void Hypervisor::handleAppTermination(int pid, int vmId, tApplicationState exitStatus)
{
    auto &vmControl = controlTable->getVmControlBlock(vmId);
    SM_UserAPP *userRequest = vmControl.request;
    auto deploymentIndex = controlTable->getAppControlBlock(vmId, pid).deploymentIndex;

    // Release the pid
    controlTable->deallocateApp(vmId, pid);
    cModule *parent = getApplicationModule(vmId, pid);
    appBuilder.deleteApp(parent);
    userRequest->changeStateByIndex(deploymentIndex, exitStatus);

    // If all apps finished
    if (userRequest->allAppsFinished())
    {
        auto update = userRequest->dup();
        update->setDestinationTopic(userRequest->getReturnTopic());
        update->setResult(SM_APP_Res_Accept);
        update->setVmId(controlTable->getGlobalVmId(vmId));
        update->setIsResponse(true);
        sendEvent(update);
    }
}

bool Hypervisor::messageIsCommand(SIMCAN_Message *msg)
{
    return msg->getOperation() == SM_APP_Req ||
           msg->getOperation() == SM_VM_Req ||
           msg->getOperation() == SM_ExtensionOffer ||
           msg->getOperation() == SM_App_Set;
}

void Hypervisor::sendToApp(cMessage *msg, int vmId, int pid) { send(msg, appGates.outBaseId + vmId); }

void Hypervisor::sendOrBufferToApp(Syscall *syscall)
{
    if (controlTable->vmIsSuspended(syscall->getVmId()))
        controlTable->bufferCall(syscall->getVmId(), syscall);
    else
        sendToApp(syscall, syscall->getVmId(), syscall->getPid());
}

void Hypervisor::sendToNetwork(cMessage *msg, int destination)
{
    send(L2Protocol::encapsulate(msg, hardwareManager->getIp(), destination), networkGates.outBaseId);
}

void Hypervisor::deallocateVmResources(int vmId)
{
    const VmControlBlock &block = controlTable->getVmControlBlock(vmId);

    // Force app termination
    for (auto &app : block.apps)
        handleAppTermination(app.pid, app.vmId, appFinishedTimeout);

    controlTable->deallocateVm(vmId);

    // If we're in a VM environment -> clear the assigned scheduler
    if (par("isVirtualHardware"))
    {
        CpuSchedulerRR *scheduler = getScheduler(vmId);
        hardwareManager->deallocateResources(block.vmType, scheduler->getCpuCoreIndex());
        scheduler->setIsRunning(false);

        DiskSyscall *diskRequest = new DiskSyscall();
        diskRequest->setVmId(vmId);
        diskRequest->setAbortIOs(true);
        send(diskRequest, gate("toDisk"));
    }
}

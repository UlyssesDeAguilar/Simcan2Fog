#include "UserAppBase.h"

using namespace hypervisor;

void UserAppBase::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init the state
    state = State::RUN;
    pc = 0;

    // Init module parameters
    startDelay = par("startDelay");
    connectionDelay = par("connectionDelay");
    isDistributedApp = par("isDistributedApp");
    myRank = (unsigned int)par("myRank");
    testID = par("testID").stdstringValue();
    appInstance = par("appInstance").stdstringValue();
    vmInstance = par("vmInstance").stdstringValue();
    userInstance = par("userInstance").stdstringValue();
    debugUserAppBase = par("debugUserAppBase");

    // Get the pid from "wrapper module"
    cModule *appModule = getParentModule();
    pid = appModule->par("pid");

    // Get the vmId from the AppArray
    cModule *appVector = appModule->getParentModule();
    vmId = appVector->par("vmId");

    // Init cGates
    inGate = gate("in");
    outGate = gate("out");

    // Schedule start
    scheduleExecStart();

    // Check connections
    if (!outGate->getNextGate()->isConnected())
        error("outGate is not connected");
}

void UserAppBase::scheduleExecStart()
{
    // Schedule waiting time to execute
    EV_DEBUG << "App module: " << getClassName() << " will start executing in " << startDelay << " seconds from now\n";
    event = new cMessage("EXECUTION START", Event::EXEC_START);
    scheduleAt(simTime() + startDelay, event);
}

void UserAppBase::finish()
{
    // Clear all sockets
    socketMap.clear();
    cancelAndDelete(event);
    cSIMCAN_Core::finish();
}

cGate *UserAppBase::getOutGate(cMessage *msg)
{
    // If msg arrives from the Operating System
    if (msg->getArrivalGate() == inGate)
        return outGate;

    // If gate not found!
    return nullptr;
}

void UserAppBase::processSelfMessage(cMessage *msg)
{
    run();
}

void UserAppBase::syscallFillData(Syscall *syscall, SyscallCode code)
{
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setOpCode(code);
}

void UserAppBase::sendRequestMessage(SIMCAN_Message *sm, cGate *outGate)
{
    // Record the starting time and change state
    state = State::WAIT;
    operationStart = simTime();
    cSIMCAN_Core::sendRequestMessage(sm, outGate);
}

ConnectionMode UserAppBase::mapService(StackServiceType type)
{
    if (type == UDP_IO)
        return UDP;
    else if (type == HTTP_CLIENT)
        return TCP_CLIENT;
    else
        return TCP_SERVER;
}

void UserAppBase::processRequestMessage(SIMCAN_Message *msg)
{
    // Resolver came back
    auto syscall = dynamic_cast<ResolverSyscall *>(msg);
    if (syscall)
    {
        EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " got resolver response \n";
        returnContext.result = (SyscallResult)syscall->getResult();
        returnContext.rf = syscall->getResolvedIp();
        delete syscall;
        __run();
        return;
    }

    // Something happened with the sockets
    auto event = check_and_cast<SocketIoSyscall *>(msg);
    if (event->getOpCode() == OPEN_CLI)
    {
        EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " got socket opened \n";
        AppSocket s;
        s.mode = mapService((StackServiceType)event->getKind());
        socketMap[event->getSocketFd()] = s;
        returnContext.result = OK;
        returnContext.rf = event->getSocketFd();
    }
    else
    {
        // Assuming RECV
        if (event->getResult() == OK)
        {
            EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " got incoming package \n";
            auto &s = socketMap[event->getSocketFd()];
            s.chunks.push_back(event->getPayload());

            if (state != LISTENING)
            {
                delete syscall;
                return;
            }
            else
            {
                EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " immediate dispatch \n";
                returnContext.chunk = s.chunks.front();
                s.chunks.pop_front();
            }
        }
        else
        {
            EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " socket failed \n";
            // Socket failed or returned that the peer closed!
            socketMap.erase(event->getSocketFd());
            returnContext.interrupt = true;
        }
    }

    delete syscall;
    __run();
}

void UserAppBase::processResponseMessage(SIMCAN_Message *msg)
{
    auto syscall = check_and_cast<Syscall *>(msg);

    if (state == State::RUN)
        error("Recieving response message while in RUN state");

    auto timeElapsed = simTime() - operationStart;

    switch (syscall->getOpCode())
    {
    case EXEC:
        callback->returnExec(timeElapsed, check_and_cast<SM_CPU_Message *>(syscall));
        break;
    case READ:
        callback->returnRead(timeElapsed);
        break;
    case WRITE:
        callback->returnWrite(timeElapsed);
        break;
    default:
        break;
    }

    delete syscall;
    __run();
}

void UserAppBase::__run()
{
    // Increment the program counter, set RUN state and call run() for next step
    state = State::RUN;
    bool rerun = true;

    do
    {
        pc++;
        rerun = run();
        EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "] Current PC: " << pc << " \n";
    } while (rerun);
}

// ----------------- CPU calls ----------------- //

void UserAppBase::execute(double MIs)
{
    // Prepare the system call
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();
    syscallFillData(sm_cpu, EXEC);

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance.c_str());
    sm_cpu->setUseMis(true);
    sm_cpu->setMisToExecute(MIs);
    sm_cpu->updateLength();

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending cpu request \n";

    // Send the request to the Operating System
    sendRequestMessage(sm_cpu, outGate);
}

void UserAppBase::execute(simtime_t cpuTime)
{
    // Prepare the system call
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();
    syscallFillData(sm_cpu, EXEC);

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance.c_str());
    sm_cpu->setUseTime(true);
    sm_cpu->setCpuTime(cpuTime);
    sm_cpu->updateLength();

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending cpu request \n";

    // Send the request to the Operating System
    sendRequestMessage(sm_cpu, outGate);
}

void UserAppBase::read(double bytes)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending read call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, READ);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::write(double bytes)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending write call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, WRITE);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::_exit()
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " terminated sucessfully\n";
    auto syscall = new Syscall();
    syscallFillData(syscall, EXIT);
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::abort()
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " terminated abruptly\n";
    Syscall *syscall = new Syscall();
    syscallFillData(syscall, ABORT);
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::resolve(const char *domainName)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " resolving:" << domainName << "\n";
    ResolverSyscall *syscall = new ResolverSyscall();
    syscallFillData(syscall, RESOLVE);
    syscall->setDomainName(domainName);
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::open_client(int targetPort, hypervisor::ConnectionMode mode)
{
    SocketIoSyscall *syscall = new SocketIoSyscall();
    syscallFillData(syscall, OPEN_CLI);
    syscall->setTargetPort(targetPort);
    syscall->setMode(mode);

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
             << " connecting to port:" << targetPort << " with mode: " << mode << "\n";

    sendRequestMessage(syscall, outGate);
}

void UserAppBase::open_server(int targetPort, hypervisor::ConnectionMode mode, const char *serviceName)
{
    SocketIoSyscall *syscall = new SocketIoSyscall();
    syscallFillData(syscall, OPEN_SERV);
    syscall->setMode(mode);
    syscall->setTargetPort(targetPort);

    if (mode != UDP)
    {
        if (!serviceName)
            error("Cannot bind a service into TCP mode without service name");
        syscall->setServiceName(serviceName);
    }

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
             << " serving in port:" << targetPort << " with mode: " << mode << "\n";
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::_send(int socketFd, PTR payload, uint32_t targetPort, uint32_t targetIp)
{
    // Prepare the system call
    SocketIoSyscall *syscall = new SocketIoSyscall();
    auto mode = socketMap[socketFd].mode;

    // Set PID and Context
    syscallFillData(syscall, SEND);
    syscall->setMode(mode);
    syscall->setSocketFd(socketFd);
    syscall->setPayload(payload);

    if (mode == UDP)
    {
        syscall->setTargetIp(targetIp);
        syscall->setTargetPort(targetPort);
    }
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
             << " serving in port:" << targetPort << " with mode: " << mode << "\n";
    sendRequestMessage(syscall, outGate);
}

bool UserAppBase::recv(int socketFd)
{
    auto &socketEntry = socketMap[socketFd];

    if (socketEntry.chunks.size() > 0)
    {
        // Dispatch the recieved message
        returnContext.chunk = socketEntry.chunks.front();
        socketEntry.chunks.pop_front();
        return true;
    }
    else
    {
        state = LISTENING;
        return false;
    }
}

void UserAppBase::close(int socketFd)
{
    SocketIoSyscall *syscall = new SocketIoSyscall();
    auto mode = socketMap[socketFd].mode;
    syscallFillData(syscall, CLOSE);
    syscall->setMode(mode);
    syscall->setSocketFd(socketFd);

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
             << " closing socket:" << socketFd << " with mode: " << mode << "\n";
    sendRequestMessage(syscall, outGate);
    socketMap.erase(socketFd);
}
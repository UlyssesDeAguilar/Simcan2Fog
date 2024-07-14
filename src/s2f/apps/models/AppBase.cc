#include "AppBase.h"

using namespace hypervisor;

void AppBase::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

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

    // Get needed paths
    nsPath = appVector->par("nsPath");
    parentPath = appVector->par("parentPath");

    // Init cGates
    inGate = gate("in");
    outGate = gate("out");

    // Schedule start
    scheduleExecStart();

    // Check connections
    if (!outGate->getNextGate()->isConnected())
        error("outGate is not connected");
}

void AppBase::scheduleExecStart()
{
    // Schedule waiting time to execute
    EV_DEBUG << "App module: " << getClassName() << " will start executing in " << startDelay << " seconds from now\n";
    event = new cMessage("EXECUTION START");
    scheduleAt(simTime() + startDelay, event);
}

void AppBase::finish()
{
    // Clear all sockets
    for (auto &iter : socketMap.getMap())
        iter.second->destroy();

    socketMap.deleteSockets();
    socketQueue.clear();
    cancelAndDelete(event);
    cSIMCAN_Core::finish();
}

cGate *AppBase::getOutGate(cMessage *msg)
{
    // If msg arrives from the Operating System
    if (msg->getArrivalGate() == inGate)
        return outGate;

    // If gate not found!
    return nullptr;
}

void AppBase::syscallFillData(Syscall *syscall, SyscallCode code)
{
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setOpCode(code);
}

void AppBase::handleMessage(cMessage *msg)
{
    auto packet = dynamic_cast<Packet *>(msg);
    auto cmd = dynamic_cast<Message*>(msg);

    if (packet || cmd)
    {
        auto socket = socketMap.findSocketFor(msg);
        if (socket)
            socket->processMessage(msg);
        else
            error("Message arrived for an unregistered socket");
    }
    else
    {
        cSIMCAN_Core::handleMessage(msg);
    }

}

void AppBase::sendRequestMessage(SIMCAN_Message *sm, cGate *outGate)
{
    // Record the starting time and change state
    operationStart = simTime();
    cSIMCAN_Core::sendRequestMessage(sm, outGate);
}

void AppBase::processResponseMessage(SIMCAN_Message *msg)
{
    auto syscall = check_and_cast<Syscall *>(msg);
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
}

// ----------------- CPU calls ----------------- //

void AppBase::execute(double MIs)
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

void AppBase::execute(simtime_t cpuTime)
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

void AppBase::read(double bytes)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending read call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, READ);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void AppBase::write(double bytes)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending write call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, WRITE);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void AppBase::_exit()
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " terminated sucessfully\n";
    auto syscall = new Syscall();
    syscallFillData(syscall, EXIT);
    sendRequestMessage(syscall, outGate);
}

void AppBase::abort()
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " terminated abruptly\n";
    Syscall *syscall = new Syscall();
    syscallFillData(syscall, ABORT);
    sendRequestMessage(syscall, outGate);
}

void AppBase::resolve(const char *domainName)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " resolving:" << domainName << "\n";
    resolver->resolve(domainName, this);
}

int AppBase::open(uint16_t localPort, ConnectionMode mode)
{
    ISocket *socket{};
    if (mode == SOCK_DGRAM)
    {
        auto s = new UdpSocket();
        s->setCallback(this);
        s->setOutputGate(gate("socketOut"));
        s->bind(localPort);
        socket = s;
    }
    else
    {
        auto s = new TcpSocket();
        s->setCallback(this);
        s->setOutputGate(gate("socketOut"));
        s->bind(localPort);
        socket = s;
    }

    socketMap.addSocket(socket);
    return socket->getSocketId();
}

void AppBase::connect(int socketFd, uint32_t destIp, uint16_t destPort)
{
    ISocket *sock = socketMap.getSocketById(socketFd);
    Ipv4Address address(destIp);
    auto tcpSocket = check_and_cast<TcpSocket *>(sock);
    tcpSocket->connect(address, destPort);
}

void AppBase::_send(int socketFd, Packet *packet, uint32_t destIp, uint16_t destPort)
{
    ISocket *sock = socketMap.getSocketById(socketFd);
    Ipv4Address address(destIp);

    if (destIp == 0 && destPort == 0)
    {
        auto tcpSocket = check_and_cast<TcpSocket *>(sock);
        tcpSocket->send(packet);
    }
    else
    {
        auto udpSock = check_and_cast<UdpSocket *>(sock);
        udpSock->sendTo(packet, address, destPort);
    }
}

void AppBase::close(int socketFd)
{
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
             << " closing socket:" << socketFd;

    ISocket *sock = socketMap.removeSocket(socketMap.getSocketById(socketFd));
    sock->close();
}

void AppBase::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd << " pushing into the queue\n";
    callback->handleDataArrived(socketFd, packet);
}

void AppBase::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd << " pushing into the queue\n";
    callback->handleDataArrived(socketFd, msg);
}

void AppBase::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void AppBase::socketEstablished(TcpSocket *socket)
{
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd << " pushing into the queue\n";
    callback->handleConnectReturn(socketFd, true);
}

void AppBase::socketFailure(TcpSocket *socket, int code)
{
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
       << "connection failed with code" << code << "\n";
    callback->handleConnectReturn(socketFd, false);
}

void AppBase::socketPeerClosed(TcpSocket *socket)
{
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
       << " peer closed\n";
    bool closeSocket = callback->handlePeerClosed(socketFd);

    // Close here as well!
    if (closeSocket)
        close(socketFd);
};

/*void AppBase::open_server(int targetPort, hypervisor::ConnectionMode mode, const char *serviceName)
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
}*/

void AppBase::socketClosed(UdpSocket *socket)
{
    int socketFd = socket->getSocketId();
    socketMap.removeSocket(socketMap.getSocketById(socketFd));
    delete socket;
}

void AppBase::socketClosed(TcpSocket *socket)
{
    int socketFd = socket->getSocketId();
    socketMap.removeSocket(socketMap.getSocketById(socketFd));
    delete socket;
}

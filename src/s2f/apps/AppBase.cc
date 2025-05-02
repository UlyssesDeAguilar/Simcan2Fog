#include "AppBase.h"

using namespace hypervisor;

void AppBase::initialize() {
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init module parameters
    startDelay = par("startDelay");
    connectionDelay = par("connectionDelay");
    isDistributedApp = par("isDistributedApp");
    myRank = (unsigned int) par("myRank");
    testID = par("testID");
    appInstance = par("appInstance");
    vmInstance = par("vmInstance");
    userInstance = par("userInstance");
    debugUserAppBase = par("debugUserAppBase");

    // Get the information from the "wrapper module"
    cModule *appModule = getParentModule();
    pid = appModule->par("pid");
    vmId = appModule->par("vmId");

    // If DNS resolver is enabled
    const char *resolverPath = appModule->par("resolverPath");
    if (!opp_isempty(resolverPath))
        resolverGate = getModuleByPath(resolverPath)->gate("clientIn");

    // Init cGates
    inGate = gate("in");
    outGate = gate("out");

    // Schedule start
    scheduleExecStart();
}

void AppBase::scheduleExecStart() {
    // Schedule waiting time to execute
    EV_DEBUG << "App module: " << getClassName() << " will start executing in "
                    << startDelay << " seconds from now\n";
    event = new cMessage("EXECUTION START");
    scheduleAt(simTime() + startDelay, event);
}

void AppBase::finish() {
    // Clear all sockets
    for (auto &iter : socketMap.getMap())
        iter.second->destroy();

    socketMap.deleteSockets();
    cancelAndDelete(event);
    cSIMCAN_Core::finish();
}

cGate* AppBase::getOutGate(cMessage *msg) {
    // If msg arrives from the Operating System
    if (msg->getArrivalGate() == inGate)
        return outGate;

    // If gate not found!
    return nullptr;
}

void AppBase::syscallFillData(Syscall *syscall, SyscallCode code) {
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setOpCode(code);
}

void AppBase::handleMessage(cMessage *msg) {
    auto arrivalGate = msg->getArrivalGate();

    if (arrivalGate == inGate || msg->isSelfMessage())
        cSIMCAN_Core::handleMessage(msg);
    else if (arrivalGate == gate("resolver")) {
        // DNS resolver response!
        auto response = check_and_cast<StubDnsResponse*>(msg);
        callback->handleResolutionFinished(response->getAddress(),
                response->getResult() == 0);
        delete response;
    } else {
        auto socket = socketMap.findSocketFor(msg);
        if (socket)
            socket->processMessage(msg);
        else
            error("Message arrived for an unregistered socket");
    }
}

void AppBase::sendRequestMessage(SIMCAN_Message *sm, cGate *outGate) {
    // Record the starting time and change state
    operationStart = simTime();
    cSIMCAN_Core::sendRequestMessage(sm, outGate);
}

void AppBase::processResponseMessage(SIMCAN_Message *msg) {
    auto syscall = check_and_cast<Syscall*>(msg);
    auto timeElapsed = simTime() - operationStart;

    switch (syscall->getOpCode()) {
    case EXEC:
        callback->returnExec(timeElapsed,
                check_and_cast<SM_CPU_Message*>(syscall));
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

void AppBase::execute(double MIs) {
    // Prepare the system call
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();
    syscallFillData(sm_cpu, EXEC);

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance);
    sm_cpu->setUseMis(true);
    sm_cpu->setMisToExecute(MIs);
    sm_cpu->updateLength();

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " sending cpu request \n";

    // Send the request to the Operating System
    sendRequestMessage(sm_cpu, outGate);
}

void AppBase::execute(simtime_t cpuTime) {
    // Prepare the system call
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();
    syscallFillData(sm_cpu, EXEC);

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance);
    sm_cpu->setUseTime(true);
    sm_cpu->setCpuTime(cpuTime);
    sm_cpu->updateLength();

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " sending cpu request \n";

    // Send the request to the Operating System
    sendRequestMessage(sm_cpu, outGate);
}

void AppBase::read(double bytes) {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " sending read call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, READ);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void AppBase::write(double bytes) {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " sending write call: " << bytes << "B" << "\n";
    auto syscall = new DiskSyscall();
    syscallFillData(syscall, WRITE);
    syscall->setBufferSize(bytes);
    sendRequestMessage(syscall, outGate);
}

void AppBase::_exit() {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " terminated sucessfully\n";
    auto syscall = new Syscall();
    syscallFillData(syscall, EXIT);
    sendRequestMessage(syscall, outGate);
}

void AppBase::abort() {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " terminated abruptly\n";
    Syscall *syscall = new Syscall();
    syscallFillData(syscall, ABORT);
    sendRequestMessage(syscall, outGate);
}

void AppBase::resolve(const char *domainName) {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " resolving:" << domainName << "\n";
    auto request = new StubDnsRequest();
    request->setDomain(domainName);
    request->setModuleId(getId());
    sendDirect(request, resolverGate);
}

int AppBase::open(uint16_t localPort, ConnectionMode mode,
        const char *interfaceName) {
    ISocket *socket { };
    if (mode == SOCK_DGRAM) {
        auto s = new UdpSocket();
        s->setCallback(this);
        s->setOutputGate(gate("socketOut"));

        if (opp_isempty(interfaceName))
            s->bind(localPort);
        else
            s->bind(L3AddressResolver().addressOf(this, interfaceName),
                    localPort);

        socket = s;
    } else {
        auto s = new TcpSocket();
        s->setCallback(this);
        s->setOutputGate(gate("socketOut"));

        if (opp_isempty(interfaceName))
            s->bind(localPort);
        else
            s->bind(L3AddressResolver().addressOf(this, interfaceName),
                    localPort);
        socket = s;
    }

    socketMap.addSocket(socket);
    return socket->getSocketId();
}

void AppBase::listen(int socketFd) {
    ISocket *sock = socketMap.getSocketById(socketFd);
    auto tcpSocket = check_and_cast<TcpSocket*>(sock);
    tcpSocket->listen();
}

void AppBase::connect(int socketFd, const L3Address &destIp,
        const uint16_t &destPort) {
    ISocket *sock = socketMap.getSocketById(socketFd);
    auto tcpSocket = check_and_cast<TcpSocket*>(sock);
    tcpSocket->connect(destIp, destPort);
}

void AppBase::_send(int socketFd, Packet *packet, uint32_t destIp,
        uint16_t destPort) {
    ISocket *sock = socketMap.getSocketById(socketFd);
    Ipv4Address address(destIp);

    if (destIp == 0 && destPort == 0) {
        auto tcpSocket = check_and_cast<TcpSocket*>(sock);
        tcpSocket->send(packet);
    } else {
        auto udpSock = check_and_cast<UdpSocket*>(sock);
        udpSock->sendTo(packet, address, destPort);
    }
}

void AppBase::close(int socketFd) {
    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]"
                    << " closing socket:" << socketFd;

    ISocket *sock = socketMap.removeSocket(socketMap.getSocketById(socketFd));
    sock->close();
}

void AppBase::registerService(const char *serviceName, int sockFd) {
    auto packet = new Packet();

    auto request = makeShared<ProxyServiceRequest>();
    request->setOperation(ProxyOperation::REGISTER);
    request->setService(serviceName);
    request->setSocketId(sockFd);
    request->setIp(getId());
    request->setPort(443);

    packet->insertData(request);
    send(packet, gate("socketOut"));
}

void AppBase::unregisterService(const char *serviceName, int sockFd) {
    auto packet = new Packet();

    auto request = makeShared<ProxyServiceRequest>();
    request->setOperation(ProxyOperation::UNREGISTER);
    request->setService(serviceName);
    request->setSocketId(sockFd);
    request->setIp(getId());
    request->setPort(443);

    packet->insertData(request);
    send(packet, gate("socketOut"));
}

void AppBase::socketAvailable(TcpSocket *socket,
        TcpAvailableInfo *availableInfo) {
    if (callback->handleClientConnection(availableInfo->getNewSocketId(),
            availableInfo->getRemoteAddr(), availableInfo->getRemotePort())) {
        auto newSocket = new TcpSocket(availableInfo);
        newSocket->setCallback(this);
        newSocket->setOutputGate(gate("socketOut"));
        socketMap.addSocket(newSocket);
        socket->accept(newSocket->getSocketId());
    } else {
        error("TBD: Closing socket on TCP/ACCEPT");
    }
}

void AppBase::socketDataArrived(UdpSocket *socket, Packet *packet) {
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
              << " pushing into the queue\n";
    callback->handleDataArrived(socketFd, packet);
}

void AppBase::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) {
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
              << " pushing into the queue\n";
    callback->handleDataArrived(socketFd, msg);
}

void AppBase::socketErrorArrived(UdpSocket *socket, Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void AppBase::socketEstablished(TcpSocket *socket) {
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
              << " pushing into the queue\n";
    callback->handleConnectReturn(socketFd, true);
}

void AppBase::socketFailure(TcpSocket *socket, int code) {
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd
              << "connection failed with code" << code << "\n";
    callback->handleConnectReturn(socketFd, false);
}

void AppBase::socketPeerClosed(TcpSocket *socket) {
    int socketFd = socket->getSocketId();
    EV << "Incoming message for socket: " << socketFd << " peer closed\n";
    bool closeSocket = callback->handlePeerClosed(socketFd);

    // Close here as well!
    if (closeSocket)
        close(socketFd);
}

void AppBase::socketClosed(UdpSocket *socket) {
    int socketFd = socket->getSocketId();
    socketMap.removeSocket(socketMap.getSocketById(socketFd));
    delete socket;
}

void AppBase::socketClosed(TcpSocket *socket) {
    int socketFd = socket->getSocketId();
    socketMap.removeSocket(socketMap.getSocketById(socketFd));
    delete socket;
}

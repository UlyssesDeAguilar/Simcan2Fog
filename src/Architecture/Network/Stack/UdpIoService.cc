#include "UdpIoService.h"
using namespace networkio;

Define_Module(UdpIoService);

void UdpIoService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        multiplexer = check_and_cast<StackMultiplexer *>(getModuleByPath("^.sm"));
    }
    ApplicationBase::initialize(stage);
}

void UdpIoService::finish()
{
    // Release the sockets (and the other resources)
    socketMap.deleteSockets();
}

void UdpIoService::handleStartOperation(LifecycleOperation *operation)
{
    // Empty (for now)
}

UdpSocket *UdpIoService::setUpNewSocket(networkio::CommandEvent *event)
{
    auto socket = new UdpSocket();

    // Configure the socket
    socket->setOutputGate(gate("socketOut"));
    socket->bind(event->getTargetPort());
    socket->setCallback(this);

    // Create the socket binding
    VmSocketBinding binding;
    binding.ip = event->getIp();
    binding.vmId = event->getVmId();
    binding.pid = event->getPid();

    // Bind the socket
    bindingMap[socket->getSocketId()] = binding;
    socketMap.addSocket(socket);

    return socket;
}

void UdpIoService::handleStopOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}

void UdpIoService::handleCrashOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}

void UdpIoService::handleMessageWhenUp(cMessage *msg)
{
    // Multiplex incoming messages from SIMCAN / INET
    auto command = check_and_cast<CommandEvent *>(msg);

    if (command)
        processRequest(msg);
    else
    {
        auto socket = socketMap.findSocketFor(msg);
        if (socket)
            socket->processMessage(msg);
        else
            error("Found package for unknown socket");
    }
}

void UdpIoService::processRequest(cMessage *msg)
{
    Enter_Method_Silent();
    auto command = check_and_cast<CommandEvent *>(msg);

    switch (command->getCommand())
    {
    case SOCKET_OPEN:
    {
        EV_DEBUG << "[UDP] Opening socket for app [" << command->getPid() << "]" << " [" << command->getVmId() << "]\n";
        UdpSocket *newSocket = setUpNewSocket(command);
        auto response = new IncomingEvent();
        response->EventBase::operator=(*command);
        response->setType(SOCKET_ESTABLISHED);
        response->setSocketId(newSocket->getSocketId());
        response->setKind(UDP_IO);
        multiplexer->processResponse(response);
        break;
    }
    case SOCKET_CLOSE:
    {
        EV_DEBUG << "[UDP] Closing socket for app [" << command->getPid() << "]" << " [" << command->getVmId() << "]\n";
        int socketId = command->getSocketId();
        auto socket = socketMap.getSocketById(socketId);

        bindingMap.erase(socketId);
        socketMap.removeSocket(socket);
        socket->close();
        break;
    }
    case SOCKET_SEND:
    {
        EV_DEBUG << "[UDP] Sending data for app [" << command->getPid() << "]" << " [" << command->getVmId() << "]\n";
        int socketId = command->getSocketId();
        auto socket = reinterpret_cast<UdpSocket *>(socketMap.getSocketById(socketId));
        auto packet = new Packet("UDP Data", command->getPayload());
        socket->sendTo(packet, Ipv4Address(command->getTargetIp()), command->getTargetPort());
        break;
    }
    default:
        error("Recieved unkown command!");
        break;
    }

    delete msg;
}

void UdpIoService::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    VmSocketBinding &binding = bindingMap.at(socket->getSocketId());

    auto response = new IncomingEvent();
    response->setIp(binding.ip);
    response->setVmId(binding.vmId);
    response->setPid(binding.vmId);

    response->setType(SOCKET_DATA_ARRIVED);
    response->setPayload(packet->peekData());
    response->setKind(UDP_IO);
    multiplexer->processResponse(response);
    delete packet;
}

void UdpIoService::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    error("Error in socket communication");
}

void UdpIoService::socketClosed(UdpSocket *socket)
{
    delete socket;
}

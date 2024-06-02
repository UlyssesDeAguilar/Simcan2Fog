#include "TcpBaseClient.h"

using namespace omnetpp;
using namespace inet;

Define_Module(TcpBaseClient);

void TcpBaseClient::initialize(int stage)
{
    if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        // Get the port for the remote connection
        port = par("port");

        // Retrieve the local address from the network layer
        cModule *host = getParentModule();
        localAddress = L3AddressResolver().addressOf(host);

        // Locate the multiplexer
        multiplexer = check_and_cast<StackMultiplexer *>(getModuleByPath("^.sm"));
    }

    ApplicationBase::initialize(stage);
}

void TcpBaseClient::finish()
{
    // Close and release all conexions
    socketMap.deleteSockets();
    ApplicationBase::finish();
}

void TcpBaseClient::processRequest(cMessage *msg)
{
    Enter_Method_Silent();

    auto command = check_and_cast<networkio::CommandEvent *>(msg);

    switch (command->getCommand())
    {
    case networkio::SOCKET_OPEN:
    {
        // Create a new socket
        TcpSocket *newSocket = new TcpSocket();
        int sockId = newSocket->getSocketId();

        // Setup the socket
        newSocket->setOutputGate(gate("socketOut"));
        newSocket->setCallback(this);

        // Find/Create the reference
        auto reference = findOrCreateReference(command->getPid(), command->getVmId());
        reference->references++;

        // Bind the reference
        socketReferenceMap[sockId] = reference;
        socketMap.addSocket(newSocket);

        // Connect to the requested ip/port
        Ipv4Address ip(command->getTargetPort());
        newSocket->connect(ip, port);

        break;
    }
    case networkio::SOCKET_SEND:
    {
        // Find the socket
        auto socket = static_cast<TcpSocket *>(socketMap.getSocketById(command->getSocketId()));

        // TODO: Check compatibility with older INET_AppMessage shenanigans
        // Encapsulate the message
        auto packet = new Packet("Adapter Packet", command->getPayload());

        // Send the package
        socket->send(packet);

        break;
    }
    case networkio::SOCKET_CLOSE:
    {
        // Find the socket and close it
        auto socket = socketMap.getSocketById(command->getSocketId());
        socket->close();
        break;
    }
    default:
        error("Unkown command");
        break;
    }

    delete command;
}

void TcpBaseClient::handleStartOperation(LifecycleOperation *operation)
{
    EV_INFO << "TcpBaseClient: Start\n";
}

void TcpBaseClient::handleStopOperation(LifecycleOperation *operation)
{
    error("TcpBaseClient: Stopping not supported");
}

void TcpBaseClient::handleCrashOperation(LifecycleOperation *operation)
{
    error("TcpBaseClient: Crashing not supported");
}

void TcpBaseClient::socketPeerClosed(inet::TcpSocket *socket)
{
    // Create the event
    auto event = new networkio::IncomingEvent();

    // Prepare the event
    auto reference = socketReferenceMap.at(socket->getSocketId());
    event->setType(networkio::SOCKET_PEER_CLOSED);
    event->setSocketId(socket->getSocketId());
    event->setVmId(reference->vmId);
    event->setPid(reference->pid);

    // Send the event
    multiplexer->processResponse(event);
}

void TcpBaseClient::socketFailure(inet::TcpSocket *socket, int code)
{
    // Create the event
    auto event = new networkio::IncomingEvent();

    // Prepare the event
    auto reference = socketReferenceMap.at(socket->getSocketId());
    event->setType(networkio::SOCKET_FAILURE);
    event->setSocketId(socket->getSocketId());
    event->setVmId(reference->vmId);
    event->setPid(reference->pid);

    // Send the event
    multiplexer->processResponse(event);
}

void TcpBaseClient::socketClosed(inet::TcpSocket *socket)
{
    // Recover context
    auto iter = socketReferenceMap.find(socket->getSocketId());

    // Remove and unbind the socket
    if (iter != socketReferenceMap.end())
    {
        auto reference = iter->second;
        socketReferenceMap.erase(iter);
        removeSocketFromReference(reference->pid, reference->vmId);
        socketMap.removeSocket(socket);
    }
    else
        error("Error closing socket on TcpBaseClient");

    // Delete the socket
    delete socket;
}

void TcpBaseClient::socketEstablished(inet::TcpSocket *socket)
{
    // Create the event
    auto event = new networkio::IncomingEvent();

    // Prepare the event
    auto reference = socketReferenceMap.at(socket->getSocketId());
    event->setType(networkio::SOCKET_ESTABLISHED);
    event->setSocketId(socket->getSocketId());
    event->setVmId(reference->vmId);
    event->setPid(reference->pid);

    // Send the event
    multiplexer->processResponse(event);
}

void TcpBaseClient::handleMessageWhenUp(cMessage *msg)
{
    // Retrieve and dispatch to the correct socket for processing
    auto socket = check_and_cast_nullable<TcpSocket *>(socketMap.findSocketFor(msg));
    if (socket != nullptr)
        socket->processMessage(msg);
    else
    {
        EV_ERROR << "message " << msg->getFullName() << "(" << msg->getClassName() << ") arrived for unknown socket \n";
        delete msg;
    }
}

void TcpBaseClient::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    // Retrieve socket id and map it to the vm-pid who owns it
    auto socketId = socket->getSocketId();
    auto iter = socketReferenceMap.find(socketId);

    if (iter == socketReferenceMap.end())
        error("The socket id is not found on the reference map");

    auto reference = iter->second;
    auto event = new networkio::IncomingEvent();

    event->setType(networkio::SOCKET_DATA_ARRIVED);
    event->setVmId(reference->vmId);
    event->setPid(reference->pid);
    event->setSocketId(socketId);
    event->setPayload(packet->peekData());

    multiplexer->processResponse(event);

    delete packet;
}

TcpBaseClient::VmReference *TcpBaseClient::findOrCreateReference(uint32_t pid, uint32_t vmId)
{
    // Build the key
    VmReferenceKey key;
    key.pid = pid;
    key.vmId = vmId;

    // Search the map
    auto iter = vmReferences.find(key);
    if (iter != vmReferences.end())
    {
        return &iter->second;
    }
    else
    {
        vmReferences[key].pid = pid;
        vmReferences[key].vmId = vmId;
        return &vmReferences[key];
    }
}

void TcpBaseClient::removeSocketFromReference(uint32_t pid, uint32_t vmId)
{
    // Build the key
    VmReferenceKey key;
    key.pid = pid;
    key.vmId = vmId;

    // Search the map
    auto iter = vmReferences.find(key);
    if (iter != vmReferences.end())
    {
        // Discount the socket
        iter->second.references--;

        // If zero, then erase
        if (iter->second.references == 0)
            vmReferences.erase(iter);
    }
}
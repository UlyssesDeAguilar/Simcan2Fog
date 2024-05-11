#include "TcpBaseProxy.h"

using namespace omnetpp;
using namespace inet;

void TcpBaseProxy::initialize(int stage)
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

    BaseProxy::initialize(stage);
}

void TcpBaseProxy::finish()
{
    // Close all conexions
    for (auto &s : socketMap.getMap())
        s.second->close();

    BaseProxy::finish();
}

void TcpBaseProxy::processRequest(cMessage *msg)
{
    Enter_Method_Silent();
    
    auto command = check_and_cast<networkio::Event *>(msg);

    switch (command->getCommand())
    {
    case networkio::SOCKET_OPEN:
    {
        // Parent registers the service in the service pool
        BaseProxy::processRequest(command);
        break;
    }
    case networkio::SOCKET_SEND:
    {
        // Dispatch the corresponding thread in order to send the message
        threadMap.at(command->getRips())->sendMessage(command);
        break;
    }
    case networkio::SOCKET_CLOSE:
    {
        int socketId = command->getRips();
        
        // Inyect the corresponding service name into the command
        command->setServiceName(threadMap.at(socketId)->getService()->c_str());

        // Parent registers the unregistering in the service pool
        BaseProxy::processRequest(command);

        // Find the socket
        auto socket = socketMap.getSocketById(command->getRips());

        // Close action, triggering the end of the thread and the deallocation of resources
        socket->close();
        break;
    }
    default:
        error("Unkown command");
        break;
    }

    delete command;
}

void TcpBaseProxy::handleStartOperation(LifecycleOperation *operation)
{
    EV_INFO << "Opening server socket on: " << localAddress
            << " with port: " << port << "\n";

    // Setup the socket
    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(localAddress, port);
    serverSocket.listen();
}

void TcpBaseProxy::socketClosed(inet::TcpSocket *socket)
{
    // TODO: Is this really necessary?
}

void TcpBaseProxy::handleMessageWhenUp(cMessage *msg)
{
    // Retrieve and dispatch to the correct socket for processing
    auto socket = check_and_cast_nullable<TcpSocket *>(socketMap.findSocketFor(msg));

    if (socket != nullptr)
        socket->processMessage(msg);
    else if (serverSocket.belongsToSocket(msg))
        serverSocket.processMessage(msg);
    else
    {
        EV_ERROR << "message " << msg->getFullName() << "(" << msg->getClassName() << ") arrived for unknown socket \n";
        delete msg;
    }
}

void TcpBaseProxy::socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo)
{
    // new TCP connection -- create new socket object and server process
    TcpSocket *newSocket = new TcpSocket(availableInfo);
    newSocket->setOutputGate(gate("socketOut"));
    
    // Create a new thread
    auto thread = newTcpThread();
    thread->setProxy(this);
    thread->setSocket(newSocket);
    newSocket->setCallback(thread);

    // Add to the managed socket map
    socketMap.addSocket(newSocket);
    threadMap[newSocket->getSocketId()] = std::unique_ptr<TcpBaseProxyThread>(thread);

    // Accept the connection
    socket->accept(availableInfo->getNewSocketId());
}
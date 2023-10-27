#include "MessageAdapter.h"

Define_Module(MessageAdapter);

void MessageAdapter::initialize(int stage)
{
    if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        // Retrieve the local address from the network layer
        cModule *host = getParentModule();
        localAddress = L3AddressResolver().addressOf(host);

        host->gate("moduleIn")->connectTo(gate("moduleIn"));
        gate("moduleOut")->connectTo(host->gate("moduleOut"));

        if (!gate("moduleIn")->isConnected() || !gate("moduleOut")->isConnected())
            error("Gates are not fully connected !");

        // Init the module
        moduleInId = gate("moduleIn")->getId();
        reqManager = new RequestManager(this, &socketMap);
        resManager = new ResponseManager(this, &socketMap);
    }
    ApplicationBase::initialize(stage);
}

void MessageAdapter::handleStartOperation(LifecycleOperation *operation)
{
    // Configure the server socket
    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(localAddress, 443);
    serverSocket.listen();
}

void MessageAdapter::handleStopOperation(LifecycleOperation *operation)
{
    error("MessageAdapter: Stopping not supported");
}

void MessageAdapter::handleCrashOperation(LifecycleOperation *operation)
{
    error("MessageAdapter: Crashing not supported");
}

void MessageAdapter::finish()
{
    // Close the server socket
    serverSocket.close();

    // Clear the connections
    reqManager->finish();
    resManager->finish();

    // Release the sockets
    socketMap.deleteSockets();
}

/*
    If it is a message from the module then create or
    find an active connection in order to send it to
    the destination !

    Otherwise is an incoming packet from the INET
    network --> translate it back to a SIMCAN_Message
*/
void MessageAdapter::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        error("This module doesn't schedule self messages");
    }
    else if (msg->getArrivalGateId() == moduleInId)
    {
        // Cast and check
        auto sm = dynamic_cast<SIMCAN_Message *>(msg);
        if (sm == nullptr)
            error("Non SIMCAN type message or subclass recieved !");

        // Search for an active connection and send the message
        if (sm->isResponse())
            resManager->processMessage(sm);
        else
            reqManager->processMessage(sm);
    }
    else
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
}

void MessageAdapter::socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo)
{
    TcpSocket *newSocket = new TcpSocket(availableInfo);

    EV_INFO << "Opening new connection with incoming ip: " << availableInfo->getRemoteAddr()
            << " port: " << availableInfo->getRemotePort() << endl;

    resManager->addNewConnectionSocket(newSocket);
    socketMap.addSocket(newSocket);
    newSocket->setOutputGate(gate("socketOut"));

    EV_INFO << "New socket: (" << newSocket->getLocalAddress() << " , " << newSocket->getLocalPort() << endl;

    socket->accept(availableInfo->getNewSocketId());
}

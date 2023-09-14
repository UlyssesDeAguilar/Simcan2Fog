#include "MessageAdapter.h"

Define_Module(MessageAdapter);

void MessageAdapter::initialize(int stage)
{
    // FIXME Change to handleStartOperation !
    if (stage == INITSTAGE_LOCAL)
    {
        moduleInId = gate("moduleIn")->getId();
        reqManager = new RequestManager(this,&socketMap);
        resManager = new ResponseManager(this,&socketMap);
    }
    ApplicationBase::initialize(stage);
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
        if (sm->getIsResponse())
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

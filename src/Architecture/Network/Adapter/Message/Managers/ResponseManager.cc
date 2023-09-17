#include "ResponseManager.h"

void ResponseManager::convertAndSend(int selectedSocket, SIMCAN_Message *sm)
{

    if (selectedSocket != -1)
    {
        // Retrieve socket and send
        auto socket = dynamic_cast<TcpSocket *>(socketMap->getSocketById(selectedSocket));
        if (socket->CONNECTED){

            // Clear the IP stack
            sm->popIp();
            sm->popIp();

            // Encapsulate the message
            auto chunk = makeShared<INET_AppMessage>(sm);
            auto packet = new Packet("Adapter Packet", chunk);

            socket->send(packet);
        }else
            adapter->error("Selected socket in unexpected state %s", socket->getState());
    }
    else
        adapter->error("No active connection found");
}

void ResponseManager::addNewConnectionSocket(TcpSocket *socket)
{
    // Prepare callback
    socket->setCallback(this);

    // Add to the main socket map and register the opened connection
    activeConnMap.emplace(socket->getRemoteAddress(), socket->getSocketId());
}

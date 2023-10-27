#include "BaseManager.h"

int BaseManager::searchOpenConnection(SIMCAN_Message *sm)
{
    if (sm == nullptr)
        adapter->error("Null message");

    auto sendAddr = sm->getNextIp();

    // Search if there's an active socket
    int selectedSocket;

    // See if we found it
    auto iter = activeConnMap.find(sendAddr);
    selectedSocket = iter == activeConnMap.end() ? -1 : iter->second;

    return selectedSocket;
}

void BaseManager::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    auto chunk = dynamic_cast<const INET_AppMessage*>(packet->peekData().get());

    if (chunk == nullptr)
        adapter->error("Erroneous kind of chunk inside recieved packet");

    // Add the sender IP to the stack !!
    auto sm = const_cast<SIMCAN_Message *>(chunk->getAppMessage());
    sm->addNewIp(socket->getRemoteAddress());
    adapter->sendToModule(sm);
    delete packet;
}

void BaseManager::socketClosed(TcpSocket *socket)
{
    // Unregister the socket from the mappings
    activeConnMap.erase(socket->getRemoteAddress());
    socketMap->removeSocket(socket);

    // Release the memory
    delete socket;
}

void BaseManager::finish()
{
    // Clear the map
    activeConnMap.clear();
}

#include "BaseManager.h"

int BaseManager::searchOpenConnection(SIMCAN_Message *sm)
{
    auto sendAddr = sm->getNextIp();

    // Search if there's an active socket
    int selectedSocket;

    // See if we found it
    auto iter = activeConnMap.find(sendAddr);
    selectedSocket = iter == activeConnMap.end() ? -1 : iter->second;

    return selectedSocket;
}


void BaseManager::processMessage(SIMCAN_Message *sm)
{
    convertAndSend(searchOpenConnection(sm), sm);
}

void BaseManager::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    auto data = packet->peekData();
    auto chunk = dynamic_cast<const cPacketChunk *>(data.get());
    if (chunk == nullptr)
        adapter->error("Erroneous kind of chunk inside recieved packet");
    
    // Extract the packet inside the chunk
    auto packet = chunk->getPacket();
    auto sm = dynamic_cast<SIMCAN_Message *>(packet);
    if (sm == nullptr)
        adapter->error("Received message is not a SIMCAN Message");
    
    adapter->sendToModule(sm);
}

void BaseManager::socketClosed(TcpSocket *socket)
{
    // Unregister the socket from the mappings
    activeConnMap.erase(socket->getRemoteAddress());
    socketMap->removeSocket(socket);

    // Release the memory
    delete socket;
}

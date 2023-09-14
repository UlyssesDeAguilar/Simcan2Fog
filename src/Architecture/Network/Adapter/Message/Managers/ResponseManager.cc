#include "ResponseManager.h"

void ResponseManager::convertAndSend(int selectedSocket, SIMCAN_Message *sm)
{
    // Encapsulate the message
    auto chunk = makeShared<cPacketChunk>(sm);
    auto packet = new Packet("Adapter Packet", chunk);

    if (selectedSocket != -1)
    {
        // Retrieve socket and send
        auto socket = dynamic_cast<TcpSocket *>(socketMap->getSocketById(selectedSocket));
        if (socket->CONNECTED)
            socket->send(packet);
        else if (socket->CONNECTING == socket->getState())
        {
            // FIXME probably there is a small edge case for incoming connections
            auto pendingVector = pendingMap.at(selectedSocket);
            pendingVector.push_back(packet);
        }
        else
            adapter->error("Selected socket in unexpected state %s", socket->getState());
    }
    else
        adapter->error("No active connection found");
}
#include "RequestManager.h"

void RequestManager::convertAndSend(int selectedSocket, SIMCAN_Message *sm)
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
    {
        // If no selected socket - create a new one
        TcpSocket *newSocket = new TcpSocket();
        int sockId = newSocket->getSocketId();

        newSocket->setOutputGate(adapter->gate("socketOut"));
        newSocket->setCallback(this);

        L3Address destAddr = sm->getNextIp();

        // Register the socket
        socketMap->addSocket(newSocket);
        activeConnMap.emplace(destAddr, sockId);

        // Place in sending queue the new message
        auto packetVector = new std::vector<Packet *>();
        packetVector->push_back(packet);

        // Register the socket with it's own "pending queue"
        pendingMap.emplace(sockId,packetVector);

        // FIXME We're assuming port 443 which is HTTPS
        newSocket->connect(destAddr, 443);
    }
}
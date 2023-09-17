#include "RequestManager.h"

void RequestManager::convertAndSend(int selectedSocket, SIMCAN_Message *sm)
{
    // Encapsulate the message
    auto chunk = makeShared<INET_AppMessage>(sm);
    auto packet = new Packet("Adapter Packet", chunk);

    if (selectedSocket != -1)
    {
        // Retrieve socket and send
        auto socket = dynamic_cast<TcpSocket *>(socketMap->getSocketById(selectedSocket));
        if (socket->CONNECTED)
            socket->send(packet);
        else if (socket->CONNECTING == socket->getState())
        {
            auto pendingVector = pendingMap.at(selectedSocket);
            pendingVector->push_back(packet);
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

void RequestManager::socketEstablished(TcpSocket *socket) 
{
    EV_INFO << "Connection established! Sending pending packets..." << endl;

    // Get the pending to send queue
    auto packetVector = pendingMap.at(socket->getSocketId());

    // Send each pending message
    for(int i = 0; i < packetVector->size(); i++){
        socket->send(packetVector->at(i));
    }
    // Clear the queue
    packetVector->clear();
}

void RequestManager::deletePendingPackages(int socketId)
{
    auto packageVector = pendingMap.at(socketId);

    // Release packages
    for (auto it = packageVector->begin(); it != packageVector->end(); it++)
        delete *it;
    
    // Release the vector
    delete packageVector;

    // Clear the entry
    pendingMap.erase(socketId);
}

void RequestManager::socketClosed(TcpSocket *socket)
{
    // FIXME Probably should notify the sending module that the packages were undelivered !
    // Release the pending queue
    deletePendingPackages(socket->getSocketId());
    BaseManager::socketClosed(socket);
}


void RequestManager::finish()
{
    // Release the pending messages
    for (auto it = activeConnMap.begin(); it != activeConnMap.end(); it++)
        deletePendingPackages(it->second);
    
    BaseManager::finish();
}

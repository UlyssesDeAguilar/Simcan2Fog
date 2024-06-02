#include "TcpBaseProxy.h"

void TcpBaseProxyThread::socketClosed(inet::TcpSocket *socket)
{
    // Unregister ourselves from the bindings
    proxy->socketMap.removeSocket(socket);
    // Finally release the socket
    delete socket;
}

void TcpBaseProxyThread::socketDeleted(inet::TcpSocket *socket) 
{
    // As the socket was released, we can finally let go the thread
    proxy->threadMap.erase(socket->getSocketId());
}

void TcpBaseProxyThread::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    if (service == nullptr)
    {
        // The thread must select a process to handle over the request
        selectFromPool(packet->peekData());

        // If the pool was saturated
        if (service == nullptr)
        {
            EV << "Cannot find available process to satisfy the request\n";
            EV << "Closing socket\n";
            socket->abort();
            delete packet;
            return;
        }
    }

    // The thread is already established and connected
    eventTemplate->setType(networkio::SOCKET_DATA_ARRIVED);
    auto event = eventTemplate->dup();
    event->setPayload(packet->peekData());
    proxy->multiplexer->processResponse(event);

    delete packet;
}

void TcpBaseProxyThread::sendMessage(networkio::CommandEvent *event)
{
    // Emplace into the envelope the message
    auto packet = new Packet("Adapter Packet", event->getPayload());

    // Deliver the envelope (Check all the stuff about the ip stack and everything)
    socket->send(packet);

    //delete event;
}

void TcpBaseProxyThread::socketPeerClosed(inet::TcpSocket *socket)
{
    if (service == nullptr)
    {
        // Quietly close the connection
        socket->close();
    }

    // Prepare the event
    eventTemplate->setType(networkio::SOCKET_PEER_CLOSED);
    auto event = eventTemplate->dup();

    // Send the event and close the socket
    proxy->multiplexer->processResponse(event);
    socket->close();
}

void TcpBaseProxyThread::socketFailure(inet::TcpSocket *socket, int code)
{
    if (service == nullptr)
    {
        // Quietly close the connection
        socket->close();
    }

    // Prepare the event
    eventTemplate->setType(networkio::SOCKET_FAILURE);
    auto event = eventTemplate->dup();

    // Send the event and close the socket
    proxy->multiplexer->processResponse(event);
    socket->close();
}

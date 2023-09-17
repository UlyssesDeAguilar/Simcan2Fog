#ifndef SIMCAN_EX_RESPONSE_MANAGER
#define SIMCAN_EX_RESPONSE_MANAGER

#include "BaseManager.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

class ResponseManager : public BaseManager
{
protected:
    virtual void convertAndSend(int selectedSocket, SIMCAN_Message * sm) override;
    virtual void socketEstablished(TcpSocket *socket) override {};
public:
    ResponseManager(MessageAdapter *adapter, SocketMap *socketMap) : BaseManager(adapter,socketMap) {};
    void addNewConnectionSocket(TcpSocket* socket);
};

#endif

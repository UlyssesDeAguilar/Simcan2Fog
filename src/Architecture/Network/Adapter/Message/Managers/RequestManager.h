#ifndef SIMCAN_EX_REQUEST_MANAGER
#define SIMCAN_EX_REQUEST_MANAGER

#include "BaseManager.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

class RequestManager : public BaseManager
{
protected:
    PendingMessageMap pendingMap; // Stores pending packages for each socket
    void deletePendingPackages(int socketId);
    virtual void convertAndSend(int selectedSocket, SIMCAN_Message *sm) override;
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketClosed(TcpSocket *socket) override;
public:
    RequestManager(MessageAdapter *adapter, SocketMap *socketMap) : BaseManager(adapter, socketMap){};
    virtual void finish() override;
};

#endif

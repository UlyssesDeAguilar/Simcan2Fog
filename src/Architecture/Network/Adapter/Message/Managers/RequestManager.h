#ifndef SIMCAN_EX_REQUEST_MANAGER
#define SIMCAN_EX_REQUEST_MANAGER

#include "BaseManager.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

class RequestManager : public BaseManager
{
protected:
    virtual void convertAndSend(int selectedSocket, SIMCAN_Message *sm) override;
public:
    RequestManager(MessageAdapter *adapter, SocketMap *socketMap) : BaseManager(adapter, socketMap){};
};

#endif
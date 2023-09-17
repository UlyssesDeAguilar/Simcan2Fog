#ifndef SIMCAN_EX_BASE_MANAGER
#define SIMCAN_EX_BASE_MANAGER

#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/socket/SocketMap.h"
#include "Messages/INET_AppMessage.h"
#include "Messages/SIMCAN_Message.h"

using namespace omnetpp;
using namespace inet;

typedef std::map<L3Address, int> IpSocketMap;
typedef std::map<int, std::vector<Packet *> *> PendingMessageMap;

class MessageAdapter;

class BaseManager : public TcpSocket::ICallback
{
protected:
    MessageAdapter *adapter;   // Main refrence
    SocketMap *socketMap;      // Reference to the main socket map
    IpSocketMap activeConnMap; // Registers open connections

    int searchOpenConnection(SIMCAN_Message *sm);
    virtual void convertAndSend(int selectedSocket, SIMCAN_Message *sm) = 0;

public:
    BaseManager(MessageAdapter *adapter, SocketMap *socketMap) { this->adapter = adapter; this->socketMap = socketMap; }

    void processMessage(SIMCAN_Message *sm) { convertAndSend(searchOpenConnection(sm), sm); }
    virtual void finish();

    /* Callbacks for the socket ICallback interface */
    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override;
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override { throw cRuntimeError("Unexpected connection opening"); }
    virtual void socketPeerClosed(TcpSocket *socket) override { socket->close(); } // If peer closed we should close too !
    virtual void socketClosed(TcpSocket *socket) override;

    // Must be redefined by subclass
    // virtual void socketEstablished(TcpSocket *socket) override;

    // For the time being these do not take action
    virtual void socketFailure(TcpSocket *socket, int code) override {}
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
    virtual void socketDeleted(TcpSocket *socket) override {}
};

#include "../MessageAdapter.h"

#endif

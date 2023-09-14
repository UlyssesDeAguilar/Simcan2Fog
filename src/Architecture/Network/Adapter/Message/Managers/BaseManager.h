#ifndef SIMCAN_EX_BASE_MANAGER
#define SIMCAN_EX_BASE_MANAGER

#include "Network/Adapter/Message/MessageAdapter.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

using namespace omnetpp;
using namespace inet;

typedef std::map<L3Address, unsigned int> IpSocketMap;
typedef std::map<int, std::vector<Packet *>> PendingMessageMap;

class BaseManager : public TcpSocket::ICallback
{
protected:
    MessageAdapter *adapter;      // Main refrence
    SocketMap *socketMap;         // Reference to the main socket map
    IpSocketMap activeConnMap;    // Registers open connections
    PendingMessageMap pendingMap; // Stores pending packages for each socket FIXME Maybe move it to the RequestManager class ??

    int searchOpenConnection(SIMCAN_Message *sm);
    virtual void convertAndSend(int selectedSocket, SIMCAN_Message *sm) = 0;

public:
    BaseManager(MessageAdapter *adapter, SocketMap *socketMap);

    // FIXME Doc the method
    void processMessage(SIMCAN_Message *sm);

    /* Callbacks for the socket ICallback interface */
    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override;
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo){ throw cRuntimeError("Unexpected connection opening");}
    virtual void socketPeerClosed(TcpSocket *socket) override { socket->close(); } // If peer closed we should close too !
    virtual void socketClosed(TcpSocket *socket) override;

    // Must be redefined by subclass
    virtual void socketEstablished(TcpSocket *socket) override;

    // For the time being these do not take action
    virtual void socketFailure(TcpSocket *socket, int code) override {}
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
    virtual void socketDeleted(TcpSocket *socket) override {}
};

#endif
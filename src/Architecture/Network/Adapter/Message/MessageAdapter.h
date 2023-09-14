#ifndef SIMCAN_EX_MESSAGE_ADAPTER
#define SIMCAN_EX_MESSAGE_ADAPTER

#include "inet/common/INETDefs.h"
#include "inet/common/packet/chunk/cPacketChunk.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/common/socket/SocketMap.h"
#include "Messages/SIMCAN_Message.h"
#include "Managers/RequestManager.h"
#include "Managers/ResponseManager.h"

using namespace omnetpp;
using namespace inet;


class MessageAdapter : public ApplicationBase, public TcpSocket::ICallback
{
private:
    int moduleInId;                 // Stores the ID of the gate "moduleIn"
    RequestManager *reqManager;
    ResponseManager *resManager;
    SocketMap socketMap;            // For handling all sockets
    TcpSocket serverSocket;         // For accepting incoming connections

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    
    // Callbacks for the socket ICallback interface
    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override { throw cRuntimeError("Unexpected data"); }
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override;
    virtual void socketEstablished(TcpSocket *socket) override {}
    virtual void socketPeerClosed(TcpSocket *socket) override {}
    virtual void socketClosed(TcpSocket *socket) override;  // Socket was closed
    virtual void socketFailure(TcpSocket *socket, int code) override {}
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
    virtual void socketDeleted(TcpSocket *socket) override {} // del socket

    // Handlers for the general module lifetime simulation
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
public:
    void sendToModule(SIMCAN_Message *sm);
};

#endif

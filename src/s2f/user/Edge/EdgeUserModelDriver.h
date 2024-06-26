#ifndef SIMCAN_EX_EDGE_USER_MODEL_DRIVER
#define SIMCAN_EX_EDGE_USER_MODEL_DRIVER

#include "../../user/common.h"
#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/socket/SocketMap.h"

class EdgeUserModelDriver : public inet::ApplicationBase, public inet::TcpSocket::ICallback
{
protected:
    inet::L3Address address;
    inet::TcpSocket localNet;
    inet::SocketMap globalNet;

    // Lifecycle
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override {}
    virtual void finish() override;

    // Callbacks for the socket ICallback interface
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *packet, bool urgent) override { throw cRuntimeError("Unexpected data"); }
    virtual void socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo) override { error("what?"); }
    virtual void socketClosed(inet::TcpSocket *socket) override{};
    virtual void socketEstablished(inet::TcpSocket *socket) override {}
    virtual void socketPeerClosed(inet::TcpSocket *socket) override {}
    virtual void socketFailure(inet::TcpSocket *socket, int code) override {}
    virtual void socketStatusArrived(inet::TcpSocket *socket, inet::TcpStatusInfo *status) override {}
    virtual void socketDeleted(inet::TcpSocket *socket) override {} // del socket

    // Handlers for the general module lifetime simulation
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;
};

#endif
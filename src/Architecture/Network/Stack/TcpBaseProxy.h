#ifndef SIMCAN_EX_TCP_BASE_PROXY
#define SIMCAN_EX_TCP_BASE_PROXY

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "Messages/SIMCAN_Message.h"
#include "Messages/INET_AppMessage.h"
#include "StackMultiplexer.h"
#include "NetworkIOEvent_m.h"

class TcpBaseProxyThread; // Forward Declaration

class TcpBaseProxy : public inet::ApplicationBase, public inet::TcpSocket::ICallback, public StackService
{
protected:
    /**
     * @brief Holds together the context necessary for relaying comms between TcpBaseThread and the Hypervisor
     * @details This approach assumes that the apps are single-threaded, single-process
     */
    struct ThreadReference
    {
        uint32_t pid{};             //!< Process Id
        uint32_t vmId{};            //!< VM Id
        TcpBaseProxyThread *thread; //!< The serving thread
    };

    std::map<int, ThreadReference> socketReferenceMap; // Maps sockets to threads and vice-versa

    inet::L3Address localAddress; // The address of the node
    uint32_t port;                // The remote port the server listens to, setup as NED parameter

    TcpSocket serverSocket;    // The main socket
    inet::SocketMap socketMap; // For handling all incoming connections

    StackMultiplexer *multiplexer; // For forwarding the responses from the endpoints

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    // Callbacks for the socket ICallback interface
    virtual void socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo) override { throw cRuntimeError("Unexpected connexion opening"); };
    virtual void socketEstablished(inet::TcpSocket *socket) override;
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *packet, bool urgent) override;
    virtual void socketStatusArrived(inet::TcpSocket *socket, inet::TcpStatusInfo *status) override {}

    virtual void socketPeerClosed(inet::TcpSocket *socket) override;
    virtual void socketFailure(inet::TcpSocket *socket, int code) override;
    virtual void socketClosed(inet::TcpSocket *socket) override;
    virtual void socketDeleted(inet::TcpSocket *socket) override {} // del socket

    // Handlers for the general module lifetime simulation
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

public:
    virtual void processRequest(omnetpp::cMessage *msg) override;
    friend class TcpBaseProxyThread;
};

class TcpBaseProxyThread : public TcpSocket::ICallback
{
protected:
    TcpSocket *socket;
    TcpBaseProxy::ThreadReference *context;

    virtual void socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo) override { throw cRuntimeError("Unexpected connexion opening"); };
    virtual void socketEstablished(inet::TcpSocket *socket) override;
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *packet, bool urgent) override;
    virtual void socketStatusArrived(inet::TcpSocket *socket, inet::TcpStatusInfo *status) override {}

    virtual void socketPeerClosed(inet::TcpSocket *socket) override;
    virtual void socketFailure(inet::TcpSocket *socket, int code) override;
    virtual void socketClosed(inet::TcpSocket *socket) override;
    virtual void socketDeleted(inet::TcpSocket *socket) override {} // del socket
    
public:
    TcpBaseProxyThread(TcpSocket *s, TcpBaseProxy::ThreadReference *ctx) : socket(s), context(ctx) {}
};

#endif

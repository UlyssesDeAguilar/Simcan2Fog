#ifndef SIMCAN_EX_TCP_BASE_PROXY
#define SIMCAN_EX_TCP_BASE_PROXY

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "Architecture/Network/Stack/BaseProxy.h"
#include "Messages/SIMCAN_Message.h"
#include "Messages/INET_AppMessage.h"
#include "StackMultiplexer.h"
#include "NetworkIOEvent_m.h"

class TcpBaseProxyThread; // Forward Declaration

class TcpBaseProxy : public BaseProxy, public inet::TcpSocket::ICallback
{
protected:
    using SocketThreadMap = std::map<int, std::unique_ptr<TcpBaseProxyThread>>;

    SocketThreadMap threadMap;     //!< Maps sockets to threads
    inet::L3Address localAddress;  //!< The address of the node
    uint32_t port;                 //!< The remote port the server listens to, setup as NED parameter
    TcpSocket serverSocket;        //!< The main socket
    inet::SocketMap socketMap;     //!< For handling all incoming connections
    StackMultiplexer *multiplexer; //!< For forwarding the responses from the endpoints

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    // Callbacks for the socket ICallback interface
    virtual void socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo) override;
    virtual void socketEstablished(inet::TcpSocket *socket) override {}
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *packet, bool urgent) override {}
    virtual void socketStatusArrived(inet::TcpSocket *socket, inet::TcpStatusInfo *status) override {}

    virtual void socketPeerClosed(inet::TcpSocket *socket) override {}
    virtual void socketFailure(inet::TcpSocket *socket, int code) override {}
    virtual void socketClosed(inet::TcpSocket *socket) override;
    virtual void socketDeleted(inet::TcpSocket *socket) override {} // del socket

    // Handlers for the general module lifetime simulation
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("TcpBaseProxy: Stopping not supported"); }
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("TcpBaseProxy: Crashing not supported"); }

    // Factory method so that subclasses can have their own thread class
    virtual TcpBaseProxyThread *newTcpThread() = 0;

public:
    virtual ~TcpBaseProxy() { socketMap.deleteSockets(); }
    virtual void processRequest(omnetpp::cMessage *msg) override;
    friend class TcpBaseProxyThread;
};

class TcpBaseProxyThread : public TcpSocket::ICallback
{
protected:
    const std::string *service{};                      //!< The name of the service this thread will serve
    ServiceEntry *entry{};                             //!< Pool entry reference to IP/VM/PROCESS which holds the service
    TcpSocket *socket{};                               //!< The socket this thread manages
    TcpBaseProxy *proxy{};                             //!< Reference to the "main" thread
    std::unique_ptr<networkio::IncomingEvent> eventTemplate{}; //!< Template which makes creating events faster

    virtual void socketAvailable(inet::TcpSocket *socket, inet::TcpAvailableInfo *availableInfo) override { throw cRuntimeError("Unexpected connexion opening"); };
    virtual void socketEstablished(inet::TcpSocket *socket) override {};
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *packet, bool urgent) override;
    virtual void socketStatusArrived(inet::TcpSocket *socket, inet::TcpStatusInfo *status) override {}

    virtual void socketPeerClosed(inet::TcpSocket *socket) override;
    virtual void socketFailure(inet::TcpSocket *socket, int code) override;
    virtual void socketClosed(inet::TcpSocket *socket) override;
    virtual void socketDeleted(inet::TcpSocket *socket) override;

    /**
     * @brief Selects from the pool of services
     * @details It must fill in the service/entry and prepare the message template,
     *          or leave service as nullptr if no nodes were available (connection will then be aborted)
     * @param sm The incoming request (usually presumed as a SM_REST_API)
     */
    virtual void selectFromPool(inet::Ptr<const inet::Chunk> chunk) = 0;

public:
    TcpBaseProxyThread() { eventTemplate = std::unique_ptr<networkio::IncomingEvent>(new networkio::IncomingEvent()); }
    void setSocket(TcpSocket *socket) { this->socket = socket; }
    void setProxy(TcpBaseProxy *proxy) { this->proxy = proxy; }
    const std::string *getService() { return service; }
    virtual void sendMessage(networkio::CommandEvent *event);
};

#endif

#ifndef SIMCAN_EX_HTTP_BASE_SERVICE
#define SIMCAN_EX_HTTP_BASE_SERVICE

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "Messages/SIMCAN_Message.h"
#include "Messages/INET_AppMessage.h"
#include "StackMultiplexer.h"
#include "NetworkIOEvent_m.h"

class TcpBaseClient : public inet::ApplicationBase, public inet::TcpSocket::ICallback, public StackService
{
protected:
    struct VmReference
    {
        uint32_t pid{};
        uint32_t vmId{};
        uint16_t references{};
    };

    union VmReferenceKey
    {
        uint64_t key;
        struct
        {
            uint32_t pid;
            uint32_t vmId;
        };
        friend bool operator<(const VmReferenceKey &a, const VmReferenceKey &b)
        {
            return a.key < b.key;
        }
    };

    inet::L3Address localAddress;                       // The address of the node
    uint32_t port;                                      // The remote port the client connects to, setup as NED parameter
    inet::SocketMap socketMap;                          // For handling all client connections
    StackMultiplexer *multiplexer;                      // For forwarding the responses from the endpoints
    std::map<VmReferenceKey, VmReference> vmReferences; // Table of clients who have a connection
    std::map<int, VmReference *> socketReferenceMap;    // Maps the socket id to the VmReference

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

    // Helper functions
    VmReference &findOrCreateReference(uint32_t pid, uint32_t vmId);
    void removeSocketFromReference(uint32_t pid, uint32_t vmId);

public:
    virtual void processRequest(omnetpp::cMessage *msg) override;
};

#endif

#ifndef SIMCAN_EX_APP_PROXY
#define SIMCAN_EX_APP_PROXY

#include "s2f/architecture/dns/DnsServiceSimplified.h"
#include "s2f/architecture/dns/common.h"
#include "inet/common/INETDefs.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "s2f/architecture/net/stack/proxy/RestMsg_m.h"

using namespace inet;

class AppBase;

struct ServiceEntry
{
    AppBase *app;
    int availableConnections;
};

class AppProxy : public cSimpleModule, public LifecycleUnsupported, public TcpSocket::ICallback
{
public:
    virtual void bindService(const char *serviceName, AppBase *app);
    virtual void unbindService(const char *serviceName, AppBase *app);
    virtual void notifyConnRelease(const char *serviceName, AppBase *app);
protected:
    using ServiceMap = std::map<opp_string, std::vector<ServiceEntry>>;

    ServiceMap serviceMap;          //!< Map which holds the service pools associated with the connections
    TcpSocket socket;               //!< Socket for accepting incoming requests
    SocketMap pendingComms;         //!< Pending conns to be rebound

    // Kernel lifecycle
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    // Helper, decides which app attends the conn
    virtual AppBase *findFitForRequest(const inet::Ptr<const RestMsg> &request);

    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override;
    virtual void socketPeerClosed(TcpSocket *socket) override;

    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override {};
    virtual void socketEstablished(TcpSocket *socket) override {};
    virtual void socketClosed(TcpSocket *socket) override {};
    virtual void socketFailure(TcpSocket *socket, int code) override {};
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {};
    virtual void socketDeleted(TcpSocket *socket) override {};
};

#endif

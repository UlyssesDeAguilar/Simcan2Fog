#ifndef SIMCAN_EX_DNS_SERVICE_REGISTRAR
#define SIMCAN_EX_DNS_SERVICE_REGISTRAR

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"

using namespace omnetpp;
using namespace inet;

class ServiceRegistrar : public cSimpleModule, public LifecycleUnsupported, public TcpSocket::ICallback
{
protected:
    TcpSocket socket;
    L3Address nameServerIp;
    L3Address localIp;

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override;
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketPeerClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override { error("Tcp Client: accepting connections disabled"); }
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {};
    virtual void socketDeleted(TcpSocket *socket) override {};
    virtual void socketClosed(TcpSocket *socket) override {};

public:
    void registerService(const char *domain);
    void unregisterService(const char *domain);
};

#endif

#ifndef SIMCAN_EX_DNS_RESOLVER
#define SIMCAN_EX_DNS_RESOLVER

#include "s2f/architecture/dns/DnsRequest_m.h"
#include "s2f/architecture/dns/DnsServiceSimplified.h"
#include "s2f/architecture/dns/common.h"
#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "RequestTimeout_m.h"

using namespace inet;

class DnsResolver : public ApplicationBase, public UdpSocket::ICallback
{
public:
    class ResolverCallback
    {
    public:
        virtual void handleResolverReturned(uint32_t ip, bool resolved) = 0;
    };

    void resolve(const char *domain, ResolverCallback *module);

protected:
    using RequestMap = std::map<uint16_t, ResolverCallback *>;

    RequestMap pendingRequests;
    L3Address ispResolver;
    uint16_t lastId{}; // Last reserved id (gives linearity and better likelyhood of finding a free one)
    UdpSocket socket;  // Socket for establishing communication with the DNS Servers
    RequestTimeout *timeOut = nullptr;

    // Kernel lifecycle
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    // INET lifecyle
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // Logic
    uint16_t getNewRequestId();

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
};

#endif

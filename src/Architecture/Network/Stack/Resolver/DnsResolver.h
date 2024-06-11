#ifndef SIMCAN_EX_DNS_RESOLVER
#define SIMCAN_EX_DNS_RESOLVER

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "Architecture/Network/DNS/common.h"
#include "Architecture/Network/Stack/NetworkIOEvent_m.h"
#include "Architecture/Network/Stack/StackMultiplexer.h"
#include "Messages/DnsRequest_m.h"
#include "RequestTimeout_m.h"
#include "Architecture/Network/DNS/DnsServiceSimplified.h"

using namespace inet;

class DnsResolver : public ApplicationBase, public UdpSocket::ICallback, public StackService
{
private:
protected:
    using RequestMap = std::map<uint16_t, cModule*>;

    StackMultiplexer *multiplexer; // For forwarding the responses from the endpoints
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

public:
    void resolve(const char *domain, cModule *module);
    virtual void processRequest(cMessage *msg) override;
};

#endif

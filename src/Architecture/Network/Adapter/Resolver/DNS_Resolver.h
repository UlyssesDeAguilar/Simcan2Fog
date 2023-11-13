#ifndef SIMCAN_EX_DNS_RESOLVER
#define SIMCAN_EX_DNS_RESOLVER

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "Architecture/Network/DNS/common.h"
#include "Messages/SM_ResolverRequest_m.h"

using namespace inet;
using namespace dns;

struct RequestState
{
    typedef enum
    {
        URL_RESOLUTION,
        TLD_RESOLUTION,
        NUM_STATES
    } State;

    SM_ResolverRequest *request; // Original request that the resolver recieved (it's like an API call)
    cMessage *timeOut;           // Pointer to the timeout message
    DomainName domain;           // Requested domain
    State currentState;          // Current state
    RequestState(const std::string &s, SM_ResolverRequest *r) : domain(s)
    {
        currentState = URL_RESOLUTION;
        request = r;
        timeOut = nullptr;
    }
};

class DNS_Resolver : public ApplicationBase, UdpSocket::ICallback
{
private:
protected:
    typedef std::map<uint16_t, RequestState *> RequestMap;

    uint16_t lastId;            // Last reserved id (gives linearity and better likelyhood of finding a free one)
    UdpSocket mainSocket;       // Socket for establishing communication with the DNS Servers
    UdpSocket responseSocket;   // Socket for receiving responses
    NameIpMap recordCache;      // Cache that keeps the resource records previously obtained
    RequestMap pendingRequests; // Map that keeps pending requests
    simtime_t timeOut;          // Timeout for pending requests

    /* Future idea for optimization -- technically would help reusing old events
    cMessage *timeoutPool;      // Pool for handling request timeouts
    size_t poolSize;            // Size of the timeout event pool
    */

    // Kernel lifecycle
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    // INET lifecyle
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    // Logic
    virtual void handleMessageWhenUp(cMessage *msg) override;
    void handleResolution(SM_ResolverRequest *request);
    cMessage *prepareRequestTimeOut(uint16_t requestId);
    void sendResponse(SM_ResolverRequest *request, L3Address *ip);
    void handleTimeOut(cMessage *msg);
    uint16_t getNewRequestId();

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
};
#endif

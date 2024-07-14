#ifndef SIMCAN_EX_DNS_REGISTRY_SERVICE
#define SIMCAN_EX_DNS_REGISTRY_SERVICE

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "s2f/architecture/dns/cache/DnsCache.h"
#include "s2f/architecture/dns/common.h"

using namespace omnetpp;
using namespace inet;

namespace dns
{
    class DnsRegistryService : public cSimpleModule, public LifecycleUnsupported
    {
    protected:
        enum Mode
        {
            IDLE,
            NET_SCAN
        };

        TcpSocket socket;
        DnsCache *cache{};

        long msgsRcvd;
        long msgsSent;
        long bytesRcvd;
        long bytesSent;

        virtual void sendBack(cMessage *msg);
        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return NUM_INIT_STAGES; }
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;
        void scanNetwork();
    };
};

#endif
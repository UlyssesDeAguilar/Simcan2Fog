#ifndef SIMCAN_EX_DNS_REGISTRY_SERVICE
#define SIMCAN_EX_DNS_REGISTRY_SERVICE

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/packet/ChunkQueue.h"
#include "s2f/architecture/dns/db/DnsDb.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"

namespace dns
{
    class DnsRegistryService : public omnetpp::cSimpleModule, public inet::LifecycleUnsupported
    {
    protected:
        inet::TcpSocket socket;
        DnsDb *dnsDatabase{};

        long msgsRcvd;
        long msgsSent;
        long bytesRcvd;
        long bytesSent;

        virtual void sendBack(omnetpp::cMessage *msg);
        virtual void initialize(int stage) override;
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual bool processRequest(const DnsRegistrationRequest *msg);
        virtual void refreshDisplay() const override;
        //void scanNetwork();
    };
};

#endif
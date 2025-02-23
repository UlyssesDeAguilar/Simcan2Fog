#ifndef SIMCAN_EX_DNS_SERVICE_SIMPLIFIED
#define SIMCAN_EX_DNS_SERVICE_SIMPLIFIED

#include <omnetpp.h>

#include "s2f/architecture/dns/DnsRequest_m.h"
#include "s2f/architecture/dns/db/DnsDb.h"
#include "s2f/architecture/dns/DnsCommon.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace dns
{

    class DnsServiceSimplified : public inet::ApplicationBase, inet::UdpSocket::ICallback
    {
    protected:
        inet::UdpSocket socket;
        DnsDb *dnsDatabase{};
        omnetpp::cPatternMatcher authorityMatcher;
        void processRecords(const char *domain, inet::Ptr<DnsRequest> response, const std::set<dns::ResourceRecord> &records);

    public:
        // Kernel lifecycle
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES + 1; }
        virtual void initialize(int stage) override;
        virtual void finish() override;

        // INET lifecyle
        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

        // Logic
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override { socket.processMessage(msg); }
        void handleQuery(const inet::Packet *packet, inet::Ptr<const DnsRequest> request);
        void handleNotImplemented(const inet::Packet *packet, inet::Ptr<const DnsRequest> request);
        void sendResponseTo(const inet::Packet *packet, inet::Ptr<DnsRequest> request);

        // Socket callbacks
        virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
        virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
        virtual void socketClosed(inet::UdpSocket *socket) override {}
    };
}

#endif

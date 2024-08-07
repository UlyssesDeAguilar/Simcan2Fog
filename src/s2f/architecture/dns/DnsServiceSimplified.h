#ifndef SIMCAN_EX_DNS_SERVICE_SIMPLIFIED
#define SIMCAN_EX_DNS_SERVICE_SIMPLIFIED

#include <omnetpp.h>

#include "s2f/architecture/dns/DnsRequest_m.h"
#include "s2f/architecture/dns/cache/DnsCache.h"
#include "s2f/architecture/dns/common.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"

/**
 * @brief Class that provides the DNS service
 * It relies on the XML dump from the Ipv4Configurator
 */
namespace dns
{

    class DnsServiceSimplified : public ApplicationBase, UdpSocket::ICallback
    {
    protected:
        UdpSocket socket;
        DnsCache *cache{};
        DnsRequest *prepareResponse(const DnsRequest *request);

    public:
        const ResourceRecord *processQuestion(const char *domain);

        // Kernel lifecycle
        virtual int numInitStages() const override { return NUM_INIT_STAGES + 1; }
        virtual void initialize(int stage) override;
        virtual void finish() override;

        // INET lifecyle
        virtual void handleStartOperation(LifecycleOperation *operation) override;
        virtual void handleStopOperation(LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
        virtual void handleCrashOperation(LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

        // Logic
        virtual void handleMessageWhenUp(cMessage *msg) override { socket.processMessage(msg); }
        void handleQuery(const Packet *request);
        void handleNotImplemented(const Packet *request);
        void sendResponseTo(const Packet *packet, DnsRequest *request);

        // Only makes sense when integrated with network stack
        void registerService(const char *serviceName);
        void unregisterService(const char *serviceName);

        // Socket callbacks
        virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
        virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
        virtual void socketClosed(UdpSocket *socket) override {} // Ignored, as it doesn't require any action
    };
}

#endif
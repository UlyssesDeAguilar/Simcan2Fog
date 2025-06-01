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

namespace s2f
{
    namespace dns
    {
        /**
         * @brief DNS server module
         * @details Based upon the RFC 1034/1035 with some practical simplifications
         *
         * @author Ulysses de Aguilar Gudmundsson
         * @version 2.0
         */
        class DnsServiceSimplified : public inet::ApplicationBase, inet::UdpSocket::ICallback
        {
        protected:
            inet::UdpSocket socket;                    //!< DNS server socket
            DnsDb *dnsDatabase{};                      //!< DNS database reference
            omnetpp::cPatternMatcher authorityMatcher; //!< Pattern matcher for determining if a domain falls under the authority domain of this server

            /**
             * @brief Process DNS records in order to craft a response for a query in a given domain
             * @details It also checks if the records are under the authority domain
             *
             * @param domain Domain where the records the records belong
             * @param response DNS response message
             * @param records DNS record list to be processed
             */
            void processRecords(const char *domain, inet::Ptr<DnsRequest> response, const std::set<ResourceRecord> &records);

        public:
            // Kernel lifecycle
            virtual int numInitStages() const override { return inet::NUM_INIT_STAGES + 1; }
            virtual void initialize(int stage) override;
            virtual void finish() override;

            // INET lifecyle
            virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
            virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
            virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support crashing"); }
            virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override { socket.processMessage(msg); }

            /**
             * @brief Handle an incoming DNS query
             *
             * @param packet Originating packet
             * @param request DNS query message
             */
            void handleQuery(const inet::Packet *packet, inet::Ptr<const DnsRequest> request);

            /**
             * @brief Crafts a reply for a DNS operation code that is not implemented
             *
             * @param packet Originating packet
             * @param request Originating request
             */
            void handleNotImplemented(const inet::Packet *packet, inet::Ptr<const DnsRequest> request);

            /**
             * @brief Sends a response to a DNS query
             *
             * @param packet The packet that originated the request
             * @param response The response for said request
             */
            void sendResponseTo(const inet::Packet *packet, inet::Ptr<DnsRequest> response);

            // Socket callbacks
            virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
            virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
            virtual void socketClosed(inet::UdpSocket *socket) override {}
        };
    }
}

#endif

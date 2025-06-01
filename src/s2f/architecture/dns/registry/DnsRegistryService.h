#ifndef SIMCAN_EX_DNS_REGISTRY_SERVICE
#define SIMCAN_EX_DNS_REGISTRY_SERVICE

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/packet/ChunkQueue.h"
#include "s2f/architecture/dns/db/DnsDb.h"
#include "s2f/architecture/dns/registry/DnsRegistrationRequest_m.h"

namespace s2f
{
    namespace dns
    {
        /**
         * @brief DNS registry service
         * @details This module exposes a RESTful API for DNS records CRUD operations
         *
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class DnsRegistryService : public omnetpp::cSimpleModule, public inet::LifecycleUnsupported
        {
        protected:
            inet::TcpSocket socket; //!< Server socket
            DnsDb *dnsDatabase{};   //!< DNS database reference

            virtual void sendBack(omnetpp::cMessage *msg);
            virtual void initialize(int stage) override;
            virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
            virtual void handleMessage(omnetpp::cMessage *msg) override;
            virtual bool processRequest(const DnsRegistrationRequest *msg);
        };
    }
}

#endif
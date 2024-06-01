//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef SIMCAN_EX_DNSQUERIERSERVERSERVICE_H_
#define SIMCAN_EX_DNSQUERIERSERVERSERVICE_H_

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "Architecture/Network/DNS/common.h"
#include "Messages/DnsRequest_m.h"
#include "DnsCache.h"

namespace dns
{
    struct ParsedQuery
    {
        opp_string tld;
        opp_string ns;
        opp_string domain;
    };
    
    using DependenciesList = std::unique_ptr<std::list<std::unique_ptr<ParsedQuery>>>;

    class DnsQuerierService : public ApplicationBase, UdpSocket::ICallback
    {
    protected:
        struct Request
        {
            uint16_t id;
            DependenciesList dependencies{};
        };

        DnsCache *cache;
        int serverGateId;
        std::map<opp_string, Request> pendingRequests;
        std::set<opp_string> questions;

    public:
        UdpSocket serverSocket;
        uint16_t queryId{};

        // Kernel lifecycle
        virtual int numInitStages() const override { return NUM_INIT_STAGES + 1; }
        virtual void initialize(int stage) override;
        virtual void finish() override;

        // INET lifecyle
        virtual void handleStartOperation(LifecycleOperation *operation) override;
        virtual void handleStopOperation(LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
        virtual void handleCrashOperation(LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

        // Logic
        virtual void handleMessageWhenUp(cMessage *msg) override;
        void handleNewQuestion(cMessage *msg);
        void processQuestion(ParsedQuery *query);

        void resolveNextStage(const char *domain, const L3Address &targetIp, ParsedQuery *query);
        void resolveNextStage(const char *domain, const L3Address &targetIp, DependenciesList list);

        uint16_t getNewQueryId();
        DnsRequest *sendRequest(const char *domain, const L3Address &address);
        DnsRequest *sendRequest(DependenciesList list, const L3Address &address);
        void processNextStage(ParsedQuery *query, DependenciesList list);

        // Socket callbacks
        virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
        virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
        virtual void socketClosed(UdpSocket *socket) override {} // Ignored, as it doesn't require any action
    };

};

#endif

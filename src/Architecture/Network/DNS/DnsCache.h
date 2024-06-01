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

#ifndef SIMCAN_EX_DNSCACHE_H_
#define SIMCAN_EX_DNSCACHE_H_

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "Architecture/Network/DNS/common.h"

namespace dns
{
    using DomainCache = std::map<opp_string, ResourceRecord>;

    class DnsCache : public cSimpleModule
    {
    protected:
        DomainCache tldCache;
        DomainCache nsCache;
        DomainCache ipCache;

    public:
        // Kernel lifecycle
        virtual void initialize() override {};
        virtual void finish() override;
        bool hasTldDomain(const char *domain) { return tldCache.find(domain) != tldCache.end(); }
        bool hasNsDomain(const char *domain) { return nsCache.find(domain) != nsCache.end(); }
        bool hasDomainCached(const char *domain) { return ipCache.find(domain) != ipCache.end(); }
        void insertData(const ResourceRecord &record);
        const ResourceRecord &getCachedDomain(const char *domain) { return ipCache.at(domain); }
        const L3Address &getNsCachedIp(const char *domain) { return nsCache.at(domain).ip; }
        const L3Address &getTldCachedIp(const char *domain) { return tldCache.at(domain).ip; }
        virtual void handleMessage(cMessage *msg) override { error("This module doesn't take any messages"); }
    };

};

#endif

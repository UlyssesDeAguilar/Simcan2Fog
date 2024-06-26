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

#ifndef SIMCAN_EX_DNSRESOLVERSERVERSERVICE_H_
#define SIMCAN_EX_DNSRESOLVERSERVERSERVICE_H_

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

namespace dns
{

  class DnsResolverService : public ApplicationBase, UdpSocket::ICallback
  {
  protected:
    DnsCache *cache;
    int serverInId;
    std::map<opp_string, cQueue> pendingRequests;

  public:
    typedef void (DnsResolverService::*Handler_t)(const DnsRequest *, DnsRequest *);
    UdpSocket serverSocket;

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
    void handleUpdateFromQuerier(cMessage *msg);
    void sendResponse(const char *domain, Packet *packet, ReturnCode code);

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override {} // Ignored, as it doesn't require any action
  };

};

#endif

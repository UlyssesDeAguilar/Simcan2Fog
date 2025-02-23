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

#ifndef SIMCAN_EX_DNS_CLIENT_SERVICE_H_
#define SIMCAN_EX_DNS_CLIENT_SERVICE_H_

#include <omnetpp.h>

#include "s2f/architecture/dns/DnsRequest_m.h"
#include "s2f/architecture/dns/DnsCommon.h"
#include "s2f/architecture/dns/client/DnsClientCommand_m.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/ChunkQueue.h"

namespace dns
{
  class DnsClientService : public inet::ApplicationBase, inet::UdpSocket::ICallback
  {
  protected:
    struct RequestContext
    {
      std::vector<inet::L3Address> servers;
      int tick = 0;
      int tries = 0;
      int selectedAddress = 0;
      uint16_t id = 0;
      inet::Packet *packet = nullptr;

      ~RequestContext()
      {
        if (packet)
          delete packet;
      }

      RequestContext(DnsClientCommand *request) : id(request->getRequestId())
      {
        servers.reserve(request->getIpPoolArraySize());
        for (int i = 0; i < request->getIpPoolArraySize(); i++)
          servers.push_back(request->getIpPool(i));
        packet = request->removePayload();
      }

      bool updateWithFailedAttempt(int maxAttempts)
      {
        if (tries >= maxAttempts)
        {
          selectedAddress++;
          if (selectedAddress >= servers.size())
            return false;
          tries = 0;
        }

        return true;
      }
    };

    omnetpp::cMessage *tickTimer;
    inet::UdpSocket socket;
    std::map<uint16_t, RequestContext> requestMap;
    std::set<inet::L3Address> failedServers;
    int timePerTick;
    int ticksForTimeOut;
    int maxAttempts;

    // Kernel lifecycle
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES + 1; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    // INET lifecyle
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

    // Logic
    virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
    void handleResponse(inet::Ptr<const DnsRequest> response, inet::Packet* packet);
    void handleRequest(DnsClientCommand *request);
    bool sendRequest(RequestContext &context);
    void sendResponse(uint16_t id, DnsClientResult result, inet::Packet *payload = nullptr);
    void processRequests();

    // Socket callbacks
    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override {} // Ignored, as it doesn't require any action

  public:
    // Constructor / Destructor
    DnsClientService();
    virtual ~DnsClientService();
  };

};

#endif

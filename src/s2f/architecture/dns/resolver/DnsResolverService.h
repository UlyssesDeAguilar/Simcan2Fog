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
#include "s2f/architecture/dns/db/DnsDb.h"
#include "s2f/architecture/dns/DnsCommon.h"
#include "s2f/architecture/dns/resolver/Resolution.h"
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

  class DnsResolverService : public inet::ApplicationBase, inet::UdpSocket::ICallback
  {
  protected:
    DnsDb *dnsDatabase{};
    inet::UdpSocket serverSocket;
    std::map<uint16_t, Resolution> resolutions;
    std::map<omnetpp::opp_string, uint16_t> questionMap;
    std::map<uint16_t, std::set<uint16_t>> requestMap;
    uint16_t lastId;
    uint16_t lastQuestionId;

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
    void handleRequest(inet::Packet *packet, inet::Ptr<const DnsRequest> request);
    void handleResponse(DnsClientIndication *msg);

    void processResolution(Resolution &resolution, bool recursion = false);
    void processAuthResolution(Resolution &resolution, const DnsTreeNode *node);
    void registerQuestion(const DnsQuestion &question, const DnsTreeNode *node, uint16_t id);
    void markFailedResolutions(const std::set<uint16_t> &failedResolutions, ReturnCode returnCode);

    void makeTransition(Resolution &resolution, ResolutionState newState);
    void sendResponse(Resolution &resolution);

    // Socket callbacks
    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override {} // Ignored, as it doesn't require any action
  };

};

#endif

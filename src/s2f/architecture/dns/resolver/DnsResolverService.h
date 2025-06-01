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

namespace s2f
{
  namespace dns
  {

    /**
     * @brief DNS Resolver service module
     * 
     * @author Ulysses de Aguilar Gudmundsson
     * @version 1.0
     */
    class DnsResolverService : public inet::ApplicationBase, inet::UdpSocket::ICallback
    {
    protected:
      DnsDb *dnsDatabase{};                                //!< Reference to the DNS database
      inet::UdpSocket serverSocket;                        //!< DNS server socket
      std::map<uint16_t, Resolution> resolutions;          //!< Resolution map, the request id is the key for fast lookup
      std::map<omnetpp::opp_string, uint16_t> questionMap; //!< Question map, used for aggregation of questions
      std::map<uint16_t, std::set<uint16_t>> requestMap;   //!< Request map, used for aggregation of requests
      uint16_t lastId;                                     //!< Last request id
      uint16_t lastQuestionId;                             //!< Last question id

      // Kernel lifecycle
      virtual int numInitStages() const override { return inet::NUM_INIT_STAGES + 1; }
      virtual void initialize(int stage) override;
      virtual void finish() override;

      // INET lifecyle
      virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
      virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
      virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

      // Socket callbacks
      virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
      virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
      virtual void socketClosed(inet::UdpSocket *socket) override {} // Ignored, as it doesn't require any action
      
      // Logic
      virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
      
      /**
       * @brief Handles an incoming DNS request
       * 
       * @param packet The encapsulating packet
       * @param request The DNS request
       */
      void handleRequest(inet::Packet *packet, inet::Ptr<const DnsRequest> request);

      /**
       * @brief Handles the response of the DnsClient
       * 
       * @param msg DnsClientIndication with a response or not
       */
      void handleResponse(DnsClientIndication *msg);

      /**
       * @brief Processes a resolution context
       * 
       * @param resolution The resolution to process
       * @param recursion Recursion flag, used for CNAME resolution
       */
      void processResolution(Resolution &resolution, bool recursion = false);
      
      /**
       * @brief Processes a resolution context for a authoritative resolution
       * 
       * @param resolution The resolution to process
       * @param node The node to process
       */
      void processAuthResolution(Resolution &resolution, const DnsTreeNode *node);
      
      /**
       * @brief Registers a question to be resolved
       * 
       * @param question Question to be resolved
       * @param node The DnsTreeNode that represents the zone
       * @param id The request id
       */
      void registerQuestion(const DnsQuestion &question, const DnsTreeNode *node, uint16_t id);

      /**
       * @brief Marks a set of resolutions as failed
       * 
       * @param failedResolutions Failed resolutions 
       * @param returnCode Return code to include in the response (NXDOMAIN or SERVFAIL)
       */
      void markFailedResolutions(const std::set<uint16_t> &failedResolutions, ReturnCode returnCode);

      /**
       * @brief Makes a transition to a new state
       * 
       * @param resolution The resolution to transition
       * @param newState The new state
       */
      void makeTransition(Resolution &resolution, ResolutionState newState);
      
      /**
       * @brief Sends a response for a resolution
       * @throws Error if the resolution is not finished
       * @param resolution The resolution context
       */
      void sendResponse(Resolution &resolution);
    };

  }
}

#endif /* SIMCAN_EX_DNSRESOLVERSERVERSERVICE_H_ */

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

namespace s2f
{
  namespace dns
  {
    /**
     * @brief Module that implements a retransmitting DNS client
     *
     * @author Ulysses de Aguilar Gudmundsson
     * @version 1.0
     */
    class DnsClientService : public inet::ApplicationBase, inet::UdpSocket::ICallback
    {
    protected:
      /**
       * @brief Struct that holds the state of a DNS request
       *
       * @author Ulysses de Aguilar Gudmundsson
       * @version 1.0
       */
      struct RequestContext
      {
        std::vector<inet::L3Address> servers; //!< List of DNS servers, presumably DNS resolvers
        int tick = 0;                         //!< Accumulated ticks
        int tries = 0;                        //!< Number of attempts of the current server
        int selectedAddress = 0;              //!< Index of the current server
        uint16_t id = 0;                      //!< Id of the request
        inet::Packet *packet = nullptr;       //!< Packet containing the DNS query to be sent

        /**
         * @brief Destructor
         */
        ~RequestContext() { delete packet; }

        /**
         * @brief Constructor
         *
         * @param request Packet containing the DNS query to be sent
         */
        RequestContext(DnsClientCommand *request) : id(request->getRequestId())
        {
          servers.reserve(request->getIpPoolArraySize());
          for (int i = 0; i < request->getIpPoolArraySize(); i++)
            servers.push_back(request->getIpPool(i));
          packet = request->removePayload();
        }

        /**
         * @brief Updates the try count of the current server
         * @details If said count exceeds the maximum number of attempts, the next server is selected.
         * This method will return true if there's still a server with enough tries left
         *
         * @param maxAttempts The maximum number of attempts
         * @return bool Whether there are still other servers to try
         */
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

      omnetpp::cMessage *tickTimer;                  //!< Timer that ticks every timePerTick milliseconds
      inet::UdpSocket socket;                        //!< Socket used to send/receive DNS requests
      std::map<uint16_t, RequestContext> requestMap; //!< Map of pending requests
      std::set<inet::L3Address> failedServers;       //!< Cache of failed servers
      int timePerTick;                               //!< Time, in milliseconds, that passes between ticks
      int ticksForTimeOut;                           //!< Number of ticks before a request times out
      int maxAttempts;                               //!< Number of attempts before a query to a server is considered failed

      // Kernel lifecycle
      virtual int numInitStages() const override { return inet::NUM_INIT_STAGES + 1; }
      virtual void initialize(int stage) override;
      virtual void finish() override;

      // INET lifecyle
      virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
      virtual void handleStopOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
      virtual void handleCrashOperation(inet::LifecycleOperation *operation) override { error("This module doesn't support crashing"); }
      virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;

      /**
       * @brief Handles the reply of a DNS request
       *
       * @param response The received DNS response
       * @param packet The encapsulating packet, used as context
       */
      void handleResponse(inet::Ptr<const DnsRequest> response, inet::Packet *packet);

      /**
       * @brief Handles a DnsClientCommand
       * @param request The request received
       */
      void handleRequest(DnsClientCommand *request);

      /**
       * @brief Send a DNS request based upon a Request context
       *
       * @param context The request context
       * @return bool Whether the request was sent. If false it implies that the request failed
       */
      bool sendRequest(RequestContext &context);

      /**
       * @brief Send response to the client module
       *
       * @param id Id of the DNS query
       * @param result The result of the resolution (OK, ERROR)
       * @param payload The response packet or nullptr otherwise (on error response)
       */
      void sendResponse(uint16_t id, DnsClientResult result, inet::Packet *payload = nullptr);

      /**
       * @brief Method called to process requests
       */
      void processRequests();

      // Socket callbacks
      virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
      virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
      virtual void socketClosed(inet::UdpSocket *socket) override {} // Ignored, as it doesn't require any action

    public:
      /**
       * @brief Constructor
       */
      DnsClientService();

      /**
       * @brief Destructor
       */
      virtual ~DnsClientService();
    };
  }
}

#endif

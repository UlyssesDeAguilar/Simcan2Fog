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

#ifndef __SIMCAN2FOG_MESSAGEQUEUEOUT_H_
#define __SIMCAN2FOG_MESSAGEQUEUEOUT_H_

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "s2f/messages/INET_AppMessage_m.h"

#include <omnetpp.h>

using namespace omnetpp;
using namespace inet;

/**
 * @brief This service manages queue handling and message dispatching
 * @details This module registers topics, handles queues for said topics
 * and dispatches messages to the appropriate consumers
 *
 * @author Ulysses de Aguilar Gudmundsson
 * @version 1.0
 */
class MessageQueueOut : public cSimpleModule
{
protected:
  /**
   * @brief Small struct that represents a consumer
   */
  struct Consumer
  {
    int socketId = -1; //!< The socket id of the consumer endpoint
    bool ready = true; //!< Flag that indicates if the consumer is ready
  };

  TcpSocket socket;                                  //!< TCP socket where the service will listen
  ChunkQueue processingQueue;                        //!< Queue that reassembles TCP packages
  bool preemptiveAlloc;                              //!< Flag for preemptive queue allocation mode
  std::map<opp_string, ChunkQueue> topicToQueueMap;  //!< Map that relates topics (strings) to queues
  std::map<opp_string, Consumer> topicToConsumerMap; //!< Map that relates topics (strings) to consumers

  virtual void initialize(int stage) override;
  virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
  virtual void handleMessage(cMessage *msg) override;
  virtual void finish() override;

  /**
   * @brief Handles a incoming request from a consumer client
   *
   * @param socketId The socket id that represents the session with the consumer
   * @param msg The application message
   */
  virtual void handleIncomingMessage(int socketId, const Ptr<const INET_AppMessage> &msg);

  /**
   * @brief Create or find a queue for a given topic
   *
   * @param destinationTopic The topic to be searched or created
   * @return ChunkQueue& Reference to the corresponding queue
   */
  virtual ChunkQueue &createOrFindQueue(const char *destinationTopic);

  /**
   * @brief Register a consumer for a given topic
   *
   * @param destinationTopic The topic to be registered
   * @param consumer The consumer to be registered
   */
  virtual void registerConsumerToTopic(const char *destinationTopic, Consumer &consumer);

  /**
   * @brief Attempt to send a message to a consumer
   *
   * @param destinationTopic The topic related
   * @param queue The queue containing the messages to be sent
   */
  virtual void attemptToSend(const char *destinationTopic, ChunkQueue &queue);

  /**
   * @brief Attempt to send a message to a consumer
   *
   * @param destinationTopic The topic related
   */
  virtual void attemptToSend(const char *destinationTopic);

  /**
   * @brief Sends back a packet/message to the client or the TCP layer
   *
   * @param msg Message to be sent
   * @param socketId The socket id
   */
  virtual void sendBack(cMessage *msg, int socketId);

public:
  /**
   * @brief Push an incoming message to the queue belonging to a topic
   * @details If the preemtive allocation mode is enabled then it won't trigger a error if there are no consumers yet
   *
   * @param destinationTopic The destination topic for the message
   * @param msg The message itself
   */
  virtual void pushIncomingMessage(const char *destinationTopic, const Ptr<const INET_AppMessage> &msg);
};

#endif

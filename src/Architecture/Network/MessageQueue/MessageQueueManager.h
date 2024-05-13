/**
 * @file MessageQueueManager.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Definition of the QueueManager for the main simulator queueing service
 * @version 0.1
 * @date 2024-05-12
 *
 *
 */

#ifndef SIMCAN_EX_MESSAGEQUEUEMANAGER_H_
#define SIMCAN_EX_MESSAGEQUEUEMANAGER_H_

#include <omnetpp.h>
#include "inet/common/packet/Packet.h"
#include "Architecture/Network/MessageQueue/Internals/SinkDelegate.h"

using namespace omnetpp;

/**
 * @brief Manages the queues and topics inside the SimcanMessageQueue
 */
class MessageQueueManager : public cSimpleModule
{
protected:
  using QueueMap = std::map<std::string, int>;
  QueueMap map;

  virtual void initialize();
  virtual void handleMessage(cMessage *msg) { error("This module doesn't handle messages"); }
  
public:
  SinkDelegate * registerOrBindTopic(const std::string &topic, cGate *inputGate);
  void publishToTopic(const std::string &topic, inet::Packet *packet);
};

#endif

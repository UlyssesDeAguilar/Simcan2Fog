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


class MessageQueueOut : public cSimpleModule
{
  protected:
    struct Consumer
    {
        int socketId = -1;
        bool ready = true;
    };

    TcpSocket socket;
    ChunkQueue processingQueue;
    bool preemptiveAlloc;
    std::map<opp_string, ChunkQueue> topicToQueueMap;
    std::map<opp_string, Consumer> topicToConsumerMap;

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleIncomingMessage(int socketId, const Ptr<const INET_AppMessage> &msg);
    virtual ChunkQueue& createOrFindQueue(const char *destinationTopic);
    virtual void registerConsumerToTopic(const char *destinationTopic, Consumer &consumer);
    virtual void attemptToSend(const char* destinationTopic, ChunkQueue &queue);
    virtual void attemptToSend(const char* destinationTopic);
    virtual void sendBack(cMessage *msg, int socketId);
    virtual void finish() override;
  public:
    virtual void pushIncomingMessage(const char* destinationTopic, const Ptr<const INET_AppMessage> &msg);
};

#endif

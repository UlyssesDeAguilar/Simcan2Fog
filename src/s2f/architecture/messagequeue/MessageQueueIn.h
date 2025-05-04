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

#ifndef __SIMCAN2FOG_MESSAGEQUEUEIN_H_
#define __SIMCAN2FOG_MESSAGEQUEUEIN_H_

#include "MessageQueueOut.h"
#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "s2f/messages/INET_AppMessage_m.h"

#include <omnetpp.h>
using namespace omnetpp;
using namespace inet;

class MessageQueueIn : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    ChunkQueue processingQueue;
    TcpSocket socket;
    MessageQueueOut *queueOut{};
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleIncomingMessage(const Ptr<const INET_AppMessage> &msg);
    virtual void sendBack(cMessage *msg, int socketId);
};

#endif

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

#include "PullService.h"

using namespace inet;
Define_Module(PullService);

void PullService::initialize()
{
    manager = check_and_cast<MessageQueueManager *>(getParentModule()->getSubmodule("manager"));
}

void PullService::handleMessage(cMessage *msg)
{
    // Recover the payload
    auto packet = check_and_cast<Packet *>(msg);
    auto payload = check_and_cast<const INET_AppMessage *>(packet->peekData().get());
    
    // Recover the destination topic
    const char *topic = payload->getAppMessage()->getSourceTopic();
    EV_TRACE << "Pull service received message for topic: " << topic << "\n";

    if (msg->getArrivalGate()->getId() == directInId)
    {
        // Came in from the delegate, forward to endpoint
        send(msg, gate("out"));
    }
    else
    {
        if (topicEstablished && payload->getAppMessage()->getOperation() == SM_QueueAck)
        {
            // Client got the last message, we can prepare the next one
            delegate->enableClientReady();
        }
        else
        {
            // Initialize the topic and tell the delegate we're ready
            delegate = manager->registerOrBindTopic(topic, gate("delegateIn"));
            delegate->enableClientReady();
        }
    }
}

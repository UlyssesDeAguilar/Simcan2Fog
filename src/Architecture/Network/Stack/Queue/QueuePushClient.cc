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

#include "QueuePushClient.h"

Define_Module(QueuePushClient);


void QueuePushClient::initialize()
{
    // Retrieve the topic from the parent
    const char *parentTopic = getParentModule()->par("nodeTopic");

    // If it was empty then use the auto unique strategy of the QueuePushClient
    this->parentTopic = *parentTopic ? parentTopic : getTopicFromPullClient();
}

const char *QueuePushClient::getTopicFromPullClient(){
    auto pullClient = check_and_cast<QueuePullClient *>(getParentModule()->getSubmodule("pullClient"));
    return pullClient->getParentTopic();
}

void QueuePushClient::handleMessage(cMessage *msg)
{
    // Check that it's a SIMCAN message
    auto sm = check_and_cast<SIMCAN_Message*>(msg);
    // Set the return topic
    sm->setReturnTopic(parentTopic);
    // Wrap into a packet
    auto packet = new Packet("Queue Push Client Request", makeShared<INET_AppMessage>(sm));
    // Send to the socket
    send(packet, "netOut");
}

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

#include "PushClient.h"

Define_Module(PushClient);
using namespace inet;

void PushClient::initialize(int stage)
{
    switch (stage)
    {
    case SimCanInitStages::NEAR:
        // Retrieve the topic from the parent
        this->parentTopic = getParentModule()->getParentModule()->par("nodeTopic");
        break;
    default:
        break;
    }
}

void PushClient::handleMessage(cMessage *msg)
{
    // Check that it's a SIMCAN message
    auto sm = check_and_cast<SIMCAN_Message *>(msg);
    EV << "Pushing message: " << msg->getClassName() << " to topic: " << sm->getDestinationTopic() << "\n";

    // Set the return topic if specified, otherwise reset to default settings
    if (sm->getAutoSourceTopic())
        sm->setReturnTopic(parentTopic);
    else
        sm->setAutoSourceTopic(true);

    // Wrap into a packet
    auto payload = makeShared<INET_AppMessage>();
    payload->setAppMessage(sm);
    auto packet = new Packet("Queue Push Client Request", payload);
    // Send to the socket
    send(packet, "netOut");
}

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

#include "PullClient.h"

#include <string>
using namespace inet;

Define_Module(PullClient);

PullClient::PullClient()
{
    signal = new SIMCAN_Message();
    signal->setOperation(SM_QueueAck);
    auto payload = makeShared<INET_AppMessage>();
    payload->setAppMessage(signal);
    ackTemplate = new Packet("Queue Pull Client Request", payload);
}

PullClient::~PullClient()
{
    delete ackTemplate;
}

void PullClient::initialize()
{
    // Retrieve the topic from the parent
    topic = par("topic");

    // Prepare the ack template
    signal->setDestinationTopic(topic);

    // Schedule auto init
    scheduleAt(simTime(), ackTemplate->dup());
}

void PullClient::handleMessage(cMessage *msg)
{
    // Part of the bootstrap of the client
    if (msg->isSelfMessage())
    {
        send(msg, gate("netOut"));
        return;
    }

    // We recieved a package from the queue, we must unwrap it
    auto packet = check_and_cast<Packet *>(msg);
    ChunkQueue queue("Queue", packet->peekData());

    while (queue.has<INET_AppMessage>())
    {
        auto encapPayload = queue.pop<INET_AppMessage>();
        auto payload = encapPayload->getAppMessage()->dup();
        take(payload);
        send(payload, gate("queueOut"));
    }

    // Send ACK, next message will come after (if there is one)
    send(ackTemplate->dup(), gate("netOut"));

    // Delete the incoming wrapping message
    delete msg;
}

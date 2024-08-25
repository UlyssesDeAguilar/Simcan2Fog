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
    ackTemplate = new Packet("Queue Pull Client Request", makeShared<INET_AppMessage>(signal));
}

PullClient::~PullClient() 
{
    delete ackTemplate;
    delete signal;
}

void PullClient::initialize(int stage)
{
    // Retrieve the topic from the parent
    cPar &parameter = getParentModule()->getParentModule()->par("nodeTopic");
    const char *parentTopic = parameter;

    // If it was empty then fill in with the module Id
    if (*parentTopic == '\0')
    {
        parameter.setStringValue(std::to_string(getId()).c_str());
        parentTopic = parameter;
    }

    this->parentTopic = parentTopic;

    // Prepare the ack template
    signal->setDestinationTopic(parentTopic);

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
    auto payload = const_cast<SIMCAN_Message *>(check_and_cast<const INET_AppMessage *>(packet->peekData().get())->getAppMessage());

    // Send to the module
    take(payload);
    send(payload, gate("queueOut"));

    // Send ACK, next message will come after (if there is one)
    send(ackTemplate->dup(), gate("netOut"));

    // Delete the incoming wrapping message
    delete msg;
}

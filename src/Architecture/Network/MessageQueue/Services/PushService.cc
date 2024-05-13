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

#include "PushService.h"
using namespace inet;
Define_Module(PushService);

void PushService::initialize()
{
    manager = check_and_cast<MessageQueueManager *>(getParentModule()->getSubmodule("manager"));
}

void PushService::handleMessage(cMessage *msg)
{
    // Recover the payload
    auto packet = check_and_cast<Packet *>(msg);
    auto payload = check_and_cast<const INET_AppMessage *>(packet->peekData().get());
    // Recover the destination topic
    const char *topic = payload->getAppMessage()->getSourceTopic();
    EV_TRACE << "Push service recieved incoming packet for topic: " << topic <<"\n";
    manager->publishToTopic(topic, packet);
}

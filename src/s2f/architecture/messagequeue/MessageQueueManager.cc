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

#include "MessageQueueManager.h"

Define_Module(MessageQueueManager);

void MessageQueueManager::initialize()
{
    WATCH_MAP(this->map);
}

void MessageQueueManager::publishToTopic(const std::string &topic, inet::Packet *packet)
{
    Enter_Method_Silent();
    auto iter = map.find(topic);

    if (iter != map.end())
    {
        EV_INFO << "Packet published to topic: " << topic << "\n";
        int queueIndex = iter->second;
        take(packet);
        cGate * destination = getParentModule()->getSubmodule("queues", queueIndex)->gate("in");
        sendDirect(packet, destination);
    }
    else
        error("Unable to publish to an unregistered topic: %s\n", topic.c_str());
}

SinkDelegate * MessageQueueManager::registerOrBindTopic(const std::string &topic, cGate *inputGate)
{
    Enter_Method_Silent();

    // Guard
    auto iter = map.find(topic);
    if (iter != map.end())
        error("For the time being the binding of topics is 1 to 1");
    
    // Build the topic queue
    cModuleType *moduleType = cModuleType::get("s2f.architecture.messagequeue.SMQueue");
    cModule *parentModule = getParentModule();

    // Create inside the vector (first resize so it will fit)
    int submoduleIndex = parentModule->getSubmoduleVectorSize("queues");
    parentModule->setSubmoduleVectorSize("queues", submoduleIndex + 1);
    auto connection = moduleType->create("queues", parentModule, queueCount);
    
    // Finish initializing and building inside
    connection->finalizeParameters();
    connection->buildInside();
    connection->callInitialize();

    // Recover output submodule and bind the gate
    auto delegate = check_and_cast<SinkDelegate *>( connection->getSubmodule("sink"));
    delegate->setDestinationGate(inputGate);

    // Register the topic
    map[topic] = queueCount;
    queueCount++;
    
    // Return the reference
    return delegate;
}

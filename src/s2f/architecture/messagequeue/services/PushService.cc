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
    manager = check_and_cast<MessageQueueManager *>(getModuleByPath("^.^.^.manager"));
}

void PushService::handleMessage(cMessage *msg)
{
    // Recover the payload
    auto packet = check_and_cast<Packet *>(msg);

    // Watch out, we might have a secuence chunk (because of TCP transmissions)
    auto sequence = dynamic_cast<const SequenceChunk *>(packet->peekData().get());
    if (sequence)
    {
        const std::deque<inet::Ptr<const inet::Chunk>> &chunks = sequence->getChunks();
        if (chunks.size() == 0)
            error("Unexpected sequence length");

        // Each chunk represents a packet (which were merged for sending)
        for (const auto &chunk : chunks)
        {
            auto m = dynamic_pointer_cast<const INET_AppMessage>(chunk);
            auto p = new Packet(packet->getName(), m);
            const char *topic = check_and_cast<const SIMCAN_Message*>(m->getAppMessage())->getDestinationTopic();
            manager->publishToTopic(topic, p);
        }

        // Delete the parent packet
        delete packet;
    }
    else
    {
        auto payload = check_and_cast<const INET_AppMessage *>(packet->peekData().get());
        // Recover the destination topic
        const char *topic = check_and_cast<const SIMCAN_Message*>(payload->getAppMessage())->getDestinationTopic();
        manager->publishToTopic(topic, packet);
    }
}

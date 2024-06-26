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

#include "../../../net/messagequeue/Internals/SinkDelegate.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/queueing/sink/ActivePacketSink.h"

using namespace inet;
using namespace inet::queueing;

Define_Module(SinkDelegate);

void SinkDelegate::initialize(int stage)
{
    PacketSinkBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
    {
        inputGate = gate("in");
        provider = findConnectedModule<IPassivePacketSource>(inputGate);
        collectionIntervalParameter = &par("collectionInterval");
        collectionTimer = new cMessage("CollectionTimer");
    }
    else if (stage == INITSTAGE_QUEUEING)
        checkPopPacketSupport(inputGate);
}

void SinkDelegate::handleMessage(cMessage *message)
{
    if (message == collectionTimer)
    {
        if (provider->canPopSomePacket(inputGate->getPathStartGate()))
        {
            // If client is ready then collect, if not try again later
            if (clientReady)
            {
                collectPacket();
                scheduleCollectionTimer();
            }
            else
                scheduleCollectionTimer();
        }
    }
    else
        throw cRuntimeError("Unknown message");
}

void SinkDelegate::scheduleCollectionTimer()
{
    scheduleAt(simTime() + collectionIntervalParameter->doubleValue(), collectionTimer);
}

void SinkDelegate::collectPacket()
{
    auto packet = provider->popPacket(inputGate->getPathStartGate());
    take(packet);
    EV_INFO << "Collecting packet " << packet->getName() << ".\n";
    numProcessedPackets++;
    processedTotalLength += packet->getDataLength();
    updateDisplayString();

    // Send to the client and await for readiness
    sendDirect(packet, destinationGate);
    clientReady = false;
}

void SinkDelegate::handleCanPopPacket(cGate *gate)
{
    Enter_Method("handleCanPopPacket");
    if (gate->getPathEndGate() == inputGate && !collectionTimer->isScheduled())
    {
        // If client is ready then collect, if not try again later
        if (clientReady)
            collectPacket();
        
        scheduleCollectionTimer();
    }
}

void SinkDelegate::enableClientReady()
{
    Enter_Method_Silent();
    EV_TRACE << "Client is ready to recieve another package\n";
    clientReady = true;
}
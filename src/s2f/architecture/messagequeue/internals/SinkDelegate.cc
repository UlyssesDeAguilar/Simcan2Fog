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

#include "SinkDelegate.h"


using namespace inet;
using namespace inet::queueing;

Define_Module(SinkDelegate);

void SinkDelegate::handleMessage(cMessage *message)
{
	if (message == collectionTimer)
	{
		if (provider == nullptr || provider->canPullSomePacket(inputGate->getPathStartGate()))
		{
			if (clientReady)
				collectPacket();
			scheduleCollectionTimer(collectionIntervalParameter->doubleValue());
		}
	}
	else
		throw cRuntimeError("Unknown message");
}

void SinkDelegate::collectPacket()
{
    auto packet = provider->pullPacket(inputGate->getPathStartGate());
    take(packet);
    emit(packetPulledSignal, packet);
    EV_INFO << "Collecting packet" << EV_FIELD(packet) << EV_ENDL;
    numProcessedPackets++;
    processedTotalLength += packet->getDataLength();
    updateDisplayString();

    // Send to the client and await for readiness
    sendDirect(packet, destinationGate);
    clientReady = false;
}

void SinkDelegate::scheduleCollectionTimerAndCollectPacket()
{
    if (!initialCollectionOffsetScheduled && initialCollectionOffset >= CLOCKTIME_ZERO) {
        scheduleCollectionTimer(initialCollectionOffset);
        initialCollectionOffsetScheduled = true;
    }
    else if (provider == nullptr || provider->canPullSomePacket(inputGate->getPathStartGate())) {
        scheduleCollectionTimer(collectionIntervalParameter->doubleValue());

        if (clientReady)
        	collectPacket();
    }
}

void SinkDelegate::enableClientReady()
{
	Enter_Method_Silent();
	EV_TRACE << "Client is ready to recieve another package\n";
	clientReady = true;
}

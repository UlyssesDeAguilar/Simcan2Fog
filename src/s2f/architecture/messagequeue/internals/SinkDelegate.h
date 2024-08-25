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

#ifndef SIMCAN_EX_SINKDELEGATE_H_
#define SIMCAN_EX_SINKDELEGATE_H_

#include <omnetpp.h>
#include "inet/queueing/sink/ActivePacketSink.h"

using namespace omnetpp;

class SinkDelegate: public inet::queueing::ActivePacketSink {
protected:
    cGate *destinationGate { };
    bool clientReady = false;

    virtual void handleMessage(cMessage *msg) override;
    virtual void scheduleCollectionTimerAndCollectPacket() override;
    virtual void collectPacket() override;
public:
    void setDestinationGate(cGate *destination) { this->destinationGate = destination; }
    void enableClientReady();

};

#endif

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
#include "inet/queueing/base/PacketSinkBase.h"
#include "inet/queueing/contract/IActivePacketSink.h"

using namespace omnetpp;

class SinkDelegate : public inet::queueing::PacketSinkBase, public inet::queueing::IActivePacketSink
{
protected:
  cGate *inputGate = nullptr;
  inet::queueing::IPassivePacketSource *provider = nullptr;

  cPar *collectionIntervalParameter = nullptr;
  cMessage *collectionTimer = nullptr;

  cGate *destinationGate{};
  bool clientReady = false;

  virtual void initialize(int stage) override;
  virtual void handleMessage(cMessage *msg) override;

  virtual void scheduleCollectionTimer();
  virtual void collectPacket();

public:
  virtual ~SinkDelegate() { cancelAndDelete(collectionTimer); }
  virtual inet::queueing::IPassivePacketSource *getProvider(cGate *gate) override { return provider; }

  virtual bool supportsPushPacket(cGate *gate) const override { return false; }
  virtual bool supportsPopPacket(cGate *gate) const override { return inputGate == gate; }
  virtual void handleCanPopPacket(cGate *gate) override;

  void setDestinationGate(cGate *destination) { this->destinationGate = destination; }
  void enableClientReady();
};

#endif

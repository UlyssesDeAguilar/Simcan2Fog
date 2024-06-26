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

#ifndef SIMCAN_EX_DISKMANAGER_H_
#define SIMCAN_EX_DISKMANAGER_H_

#include <omnetpp.h>

#include "s2f/core/cSIMCAN_Core.h"

using namespace omnetpp;

class DiskManager : public cSimpleModule
{
protected:
  struct DiskQueue
  {
    cMessage ioFinished;
    cQueue queue;
  };
  struct DiskSpecs
  {
    double readBandwidth;  //!< In Mbit/s
    double writeBandwidth; //!< In Mbit/s
  } diskSpecs;

  std::vector<DiskQueue> queueTable;

  virtual int numInitStages() const override { return NEAR + 1; }
  virtual void initialize(int stage) override;
  virtual void finish() override;
  virtual void handleMessage(cMessage *msg) override;

private:
  void flushQueue(DiskQueue &entry);
  void scheduleIo(DiskQueue &entry);

public:
  void stopVmQueue(uint32_t vmId);
};

#endif

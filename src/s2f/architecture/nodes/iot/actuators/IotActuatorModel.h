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

#ifndef __SIMCAN2FOG_IOTACTUATORMODEL_H_
#define __SIMCAN2FOG_IOTACTUATORMODEL_H_

#include <omnetpp.h>

#include "s2f/core/include/signals.h"
#include "s2f/architecture/nodes/iot/IotPayload_m.h"
#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"


using namespace omnetpp;

namespace s2f::iot
{

  class IotActuatorModel : public cSimpleModule, public inet::UdpSocket::ICallback
  {
  protected:
    inet::UdpSocket receiveSocket;
    simtime_t totalTimeOn;
    simtime_t powerOnTimestamp;
    PowerState powerState;
    cEnum *powerStateEnum{};

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::INITSTAGE_LAST; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void finish() override;
    
    virtual void changePowerState(PowerState newState);
    virtual void savePowerOnTimestamp();
    virtual void addPowerOnDelta();

  public:
    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override {};
  };

}

#endif

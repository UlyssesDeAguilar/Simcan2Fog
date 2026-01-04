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

#include "IotActuatorModel.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

using namespace s2f::iot;
using namespace inet;

Define_Module(IotActuatorModel);

void IotActuatorModel::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL)
    {
        // Initialize
        totalTimeOn = 0;
        powerOnTimestamp = 0;

        // Power state on start
        powerState = par("poweredOnStart") ? PowerState::ON : PowerState::OFF;
        emit(powerStateSignal, powerState);

        // Get the power state enum so we can print the labels
        powerStateEnum = cEnum::find("PowerState", "s2f::iot");
        if (!powerStateEnum)
            error("Error finding enum for power state logging");

        EV_DEBUG << "Power state on start: " << powerStateEnum->getStringFor(powerState) << "\n";

        // Initial setup of the socket
        receiveSocket.setOutputGate(gate("socketOut"));
        receiveSocket.setCallback(this);

        // Make the internal elements observable
        WATCH(totalTimeOn);
        WATCH(powerOnTimestamp);
        WATCH(powerState);
    }
    else if (stage == inet::INITSTAGE_APPLICATION_LAYER)
    {
        // Bind the UDP socket
        receiveSocket.bind(par("localPort"));
        EV_DEBUG << "Binding UDP socket on port: " << par("localPort") << "\n";
    }
}

void IotActuatorModel::finish()
{
    // In case the sensor was on until the end of the simulation we must add said time
    if (powerState == PowerState::ON)
        addPowerOnDelta();

    recordScalar("Total time on:", totalTimeOn, "s");
}

void IotActuatorModel::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        EV_TRACE << "Self message recieved\n";
        auto powerEvent = dynamic_cast<IotPowerEvent *>(msg);
        changePowerState(powerEvent->getPowerStateToTransition());
        delete msg;
    }
    else if (receiveSocket.belongsToSocket(msg))
    {
        EV_TRACE << "Message for socket arrived\n";
        receiveSocket.processMessage(msg);
    }
    else
    {
        EV_WARN << "Unexpected message" << msg->getClassName() << "arrived. Dropping ...\n";
        delete msg;
    }
}

void IotActuatorModel::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    ChunkPtr packetData = packet->peekData();
    auto iotCommand = dynamicPtrCast<const IotCommand>(packetData);

    EV_DEBUG << "Packet recieved from: "
             << packet->getTag<L3AddressInd>()->getSrcAddress()
             << ":"
             << packet->getTag<L4PortInd>()->getSrcPort();

    // This shouldn't happen
    if (iotCommand == nullptr)
    {
        EV_WARN << "Unexpected packet or format ... dropping\n";
        delete packet;
        return;
    }

    PowerState newState = iotCommand->getPowerState();

    // If we got a command then evaluate
    const char *newStateString = powerStateEnum->getStringFor(newState);

    EV_INFO << "Request for changing power state, current: "
            << powerStateEnum->getStringFor(powerState)
            << " to: "
            << newStateString
            << "\n";

    // Idempotent operation
    if (powerState == newState)
    {
        EV_INFO << "Already in desired state: " << newStateString << " nothing to do!\n";
        delete packet;
        return;
    }

    // Prepare and schedule power transition
    simtime_t currentTime = simTime();
    auto powerEvent = new IotPowerEvent(newStateString);
    powerEvent->setPowerStateToTransition(iotCommand->getPowerState());

    if (powerEvent->getPowerStateToTransition() == PowerState::ON)
        scheduleAt(currentTime + par("powerOnTime").doubleValueInUnit("s"), powerEvent);
    else
        scheduleAt(currentTime + par("powerOffTime").doubleValueInUnit("s"), powerEvent);

    // Release packet
    delete packet;
}

void IotActuatorModel::socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication)
{
    // Right now this shouldn't happen
    EV_WARN << "Received an socket error\n";
    delete indication;
}

void IotActuatorModel::savePowerOnTimestamp()
{
    powerOnTimestamp = simTime().inUnit(SimTimeUnit::SIMTIME_S);
}

void IotActuatorModel::addPowerOnDelta()
{
    simtime_t currentTimestamp = simTime().inUnit(SimTimeUnit::SIMTIME_S);

    // Add the difference (now - last turn on op) to the totalTimeOn
    totalTimeOn += currentTimestamp - powerOnTimestamp;
    powerOnTimestamp = 0;
}

void IotActuatorModel::changePowerState(PowerState newState)
{
    // Save the new timestamp or update the total power on time
    if (newState == PowerState::ON)
        savePowerOnTimestamp();
    else
        addPowerOnDelta();

    // Make the change and emit the signal
    powerState = newState;
    emit(powerStateSignal, powerState);

    // Update the UI showing a small bubble with the change
    if (hasGUI())
    {
        char buffer[20];
        snprintf(buffer, sizeof(buffer) * sizeof(buffer[0]), "Power state: %s", powerStateEnum->getStringFor(newState));
        getParentModule()->bubble(buffer);
    }

    return;
}

void IotActuatorModel::refreshDisplay() const
{
    cDisplayString &displayString = getParentModule()->getDisplayString();
    char buff[20];
    snprintf(buff, sizeof(buff) * sizeof(buff[0]), "Power state: %s", powerStateEnum->getStringFor(powerState));
    displayString.setTagArg("t", 0, buff);

    if (powerState == PowerState::OFF)
    {
        displayString.setTagArg("i2", 0, "status/off");
    }
    else
    {
        displayString.setTagArg("i2", 0, "status/green");
    }
}
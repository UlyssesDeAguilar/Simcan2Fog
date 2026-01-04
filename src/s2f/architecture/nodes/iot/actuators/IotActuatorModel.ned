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

package s2f.architecture.nodes.iot.actuators;
import inet.applications.contract.IApp;


simple IotActuatorModel like IApp
{
    parameters:
        int localPort = default(1000); // Port where the actuator is listening to the controller
        double powerOnTime @unit(ms) = default(200ms); // Time to power on from OFF state
        double powerOffTime @unit(ms) = default(200ms); // Time to power off from ON state
        bool poweredOnStart = default(true); // If the sensor is ON right at the start of the simulation

        @signal[powerState](type=long);
        @statistic[powerState](source=powerState;record=vector;enumname=s2f::iot::PowerState);
	gates:
	    output socketOut;
	    input socketIn;
}

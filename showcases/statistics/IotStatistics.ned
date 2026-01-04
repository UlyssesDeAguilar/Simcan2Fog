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

package s2f.showcases.statistics;

import s2f.core.simdata.SimData;
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.LuminositySensor;
import s2f.architecture.nodes.iot.actuators.SmartBulb;
import s2f.users.edge.EdgeUser;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import inet.visualizer.common.IntegratedMultiVisualizer;
import inet.node.ethernet.Eth10M;
import inet.node.wireless.AccessPoint;
import inet.node.inet.WirelessHost;
import inet.node.inet.Router;

network IotStatistics
{
    @display("bgb=20,20;bgi=showcases/floorplan,s");

    submodules:
        simData: SimData {
            @display("p=10.177515,1.2307693");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=12.591716,1.2307693");
        }

        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=18.319527,1.183432");
        }

        visualizer: IntegratedMultiVisualizer {
            @display("p=15.573964,1.2307693");
        }

        controller: Raspberry5 {
            @display("p=9.2,13.6");
        }

        user: EdgeUser {
            @display("p=14.24,15.52");
        }

        sensor1: LuminositySensor {
            @display("p=4.49,7.38");
        }

        lamp1: SmartBulb {
            @display("p=14.24,7.44;i=light64");
        }

    connections:
        controller.serialIn <-- user.serialOut;
        controller.serialOut --> user.serialIn;
}

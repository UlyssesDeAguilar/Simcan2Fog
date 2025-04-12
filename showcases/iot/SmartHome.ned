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

package iot;

import s2f.core.simdata.SimData;
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.LuminositySensor;
import s2f.users.edge.EdgeUser;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import inet.visualizer.common.IntegratedMultiVisualizer;
import inet.node.ethernet.Eth10M;
import inet.node.wireless.AccessPoint;
import inet.node.inet.WirelessHost;
import inet.node.inet.Router;

network SmartHome
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
            @display("p=6.963665,10.518036");
        }

        user: EdgeUser {

            @display("p=9.6094675,8.094674");
        }
        sensor1: LuminositySensor {
            @display("p=2.5388362,3.6994472");
        }
        sensor2: LuminositySensor {
            @display("p=11.550296,10.508876");
        }
        sensor3: LuminositySensor {
            @display("p=2.9015272,16.756319");
        }
        sensor4: LuminositySensor {
            @display("p=9.647577,16.611242");
        }

        lamp1: WirelessHost {
            @display("p=8.124276,5.150211;i=light64");
        }
        lamp2: WirelessHost {
            @display("p=17.191547,10.518036;i=light64");
        }
        lamp3: WirelessHost {
            @display("p=4.2797527,7.1812797;i=light64");
        }
        lamp4: WirelessHost {
            @display("p=13.201948,15.378094;i=light64");
        }
    connections:
        controller.serialIn <-- user.serialOut;
        controller.serialOut --> user.serialIn;
}

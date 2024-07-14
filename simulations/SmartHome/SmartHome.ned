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

package s2f.simulations.SmartHome;

import s2f.core.simdata.SimData;
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.LuminositySensor;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.integrated.IntegratedMultiVisualizer;
import inet.visualizer.integrated.IntegratedVisualizer;

import inet.node.ethernet.Eth10M;
import inet.node.wireless.AccessPoint;
import inet.node.inet.WirelessHost;
import inet.node.inet.Router;

network SmartHome
{
    @display("bgb=20,20;bgi=showcases/floorplan,s");

    submodules:
        simData: SimData {
            @display("p=10.155345,1.3782254");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=13.637177,1.233149");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=17.699316,3.9170616");
        }
        visualizer: IntegratedMultiVisualizer {
            @display("p=17.699316,1.1606108");
        }
        controller: Raspberry5 {
            @display("p=6.963665,10.518036");
            //ns.**.radioMediumModule = "^.^.^.^.radioMedium";
        }
        router: Router {
            @display("p=1.8134545,12.476566");
        }

        sensor1: LuminositySensor {
            @display("p=2.5388362,3.6994472");
        }
        sensor2: LuminositySensor {
            @display("p=10.880727,10.010268");
        }
        sensor3: LuminositySensor {
            @display("p=2.9015272,16.756319");
        }
        sensor4: LuminositySensor {
            @display("p=9.647577,16.611242");
        }

        ap: AccessPoint {
            @display("p=3.9895997,10.663113");
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
        router.ethg++ <--> Eth10M <--> ap.ethg++;
        //controller.comm <--> Eth10M <--> ap.ethg++;
}

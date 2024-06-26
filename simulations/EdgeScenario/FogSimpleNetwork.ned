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

package s2f.simulations.EdgeScenario;
import s2f.simulations.Templates.BaseNetwork;
import s2f.Core.SimData.SimData;
import s2f.Architecture.Network.DNS.DnsSimplified;
import s2f.Management.DataCentre.DataCentre;
import s2f.Architecture.Nodes.Fog.FogNode;
import s2f.Architecture.Nodes.Iot.*;
import s2f.UserModel.Edge.EdgeTile;
import s2f.UserModel.Edge.LightEdgeTile;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.internetcloud.InternetCloud;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.EtherSwitch;

import s2f.Architecture.Nodes.Iot.HumiditySensor;
import s2f.Architecture.Nodes.Iot.Raspberry5;

network SimpleFogNetwork extends BaseNetwork
{
    parameters:
        int edgeTiles = default(1);
        string serviceName = default("service");

        @display("bgb=800,400;bgg=50");
    submodules:

        dns: DnsSimplified {
            @display("p=628.125,191.955");
        }

        dc: DataCentre {
            @display("p=645.21,288.435");
        }

        fg: FogNode {
            @display("p=285.42,248.235");
        }

        local: InternetCloud {
            @display("p=194.97,247.23001");
        }

        metropolitan: InternetCloud {
            @display("p=349.74002,248.235");
        }

        backbone: InternetCloud {
            @display("p=531.645,247.23001");
        }

        sensor: HumiditySensor {
			numWlanInterfaces = 0;
            @display("p=218.085,127.635");
        }
        controller: Raspberry5 {

            @display("p=107.535,107.535");
        }
        ap: EtherSwitch {
            @display("i=device/accesspoint, p=144.72,199.99501");
        }
        actuator[1]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("i=faucet64;p=52.260002,195.975");
        }
    connections:
        sensor.ethg++ <--> Eth100M <--> ap.ethg++;
        controller.comm <--> Eth100M <--> ap.ethg++;
        actuator[0].ethg++ <--> Eth100M <--> ap.ethg++;

        ap.ethg++ <--> Eth10G <--> local.ethg++;
        local.ethg++ <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;
        backbone.ethg++ <--> Eth100G <--> dc.comm++;
        backbone.ethg++ <--> Eth100G <--> dns.ethg++;
}

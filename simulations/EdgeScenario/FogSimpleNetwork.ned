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

package simcan2.simulations.EdgeScenario;
import simcan2.Core.SimData.SimData;
import simcan2.Architecture.Network.DNS.DnsSimplified;
import simcan2.Management.DataCentre.DataCentre;
import simcan2.Architecture.Nodes.Fog.FogNode;
import simcan2.Architecture.Nodes.Iot.*;
import simcan2.UserModel.Edge.EdgeTile;
import simcan2.UserModel.Edge.LightEdgeTile;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.internetcloud.InternetCloud;
import inet.node.inet.Router;

import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;

network SimpleFogNetwork
{
    parameters:
        int edgeTiles = default(1);

        @display("bgb=800,400;bgg=50");
    submodules:
        simData: SimData {
            @display("p=623,40");
        }

        dns: DnsSimplified {
            @display("p=599,197");
        }

        dc: DataCentre {
            @display("p=707,250");
        }

        fg: FogNode {
            @display("p=153,251");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=734,34");
        }

        metropolitan: InternetCloud {

            @display("p=256,252");
        }

        backbone: InternetCloud {
            @display("p=503,253");
        }
        tile: LightEdgeTile {
            @display("p=51,250,m,4,10;is=s");
        }

    connections:
        tile.ethernet <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;
        backbone.ethg++ <--> Eth100G <--> dc.comm++;
        backbone.ethg++ <--> Eth100G <--> dns.ethg++;
}

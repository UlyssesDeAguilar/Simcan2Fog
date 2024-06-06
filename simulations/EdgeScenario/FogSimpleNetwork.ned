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
import simcan2.Architecture.Nodes.Iot.*;
import simcan2.UserModel.Edge.EdgeTile;
import simcan2.UserModel.Edge.LightEdgeTile;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

network SimpleFogNetwork
{
    parameters:
        int edgeTiles = default(1);

        @display("bgb=600,400");
    submodules:
        simData: SimData {
            @display("p=48,103");
        }

        dns: DnsSimplified {
            @display("p=301,169");
        }

        router: Router {
            @display("p=300,261");
        }

        dc: DataCentre {
            @display("p=472,256");
        }
        
        configurator: Ipv4NetworkConfigurator {
            @display("p=48,35");
        }

        tile: LightEdgeTile {
            @display("p=151,259,m,4,10;is=s");
        }

    connections:
        router.ethg++ <--> Eth10M <--> tile.ethernet;
        router.ethg++ <--> Eth10M <--> dc.comm++;
        router.ethg++ <--> Eth10M <--> dns.ethg++;
}
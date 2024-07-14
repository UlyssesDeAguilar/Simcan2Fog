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

package s2f.simulations.EdgeTesting;

import s2f.core.simdata.SimData;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;
import s2f.architecture.dns.DnsSimplified;
import s2f.users.edge.LightEdgeTile;
import s2f.users.edge.EdgeTile;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

network EdgeNetwork
{
    parameters:
        int edgeTiles = default(1);

        @display("bgb=698,437");
    submodules:
        simData: SimData {
            @display("p=48,103");
        }

        router_bb: Router {
            @display("i=abstract/router2;p=262,257");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=48,35");
        }

        tile[edgeTiles]: EdgeTile {
            @display("p=398,256,m,4,10;is=s");
        }

    connections:
        for i=0..edgeTiles-1 {
            router_bb.ethg++ <--> Eth10M <--> tile[i].ethernet;
        }
}



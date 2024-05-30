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

package simcan2.simulations.FogShowcase;

import inet.node.bgp.BgpRouter;
// Mission critical modules
import simcan2.Core.SimData.SimData;
import simcan2.Architecture.Network.MessageQueue.MessageQueue;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100G;

// Entities involved in action
import simcan2.Management.CloudProvider.CloudProvider;
import simcan2.Management.DataCentre.DataCentre;
import simcan2.Management.UserGenerators.UserGenerator;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network FogShowcase2
{
    @display("bgb=700,700;bgi=maps/europe,c;bgg=100;bgu=km");
    submodules:
        simData: SimData {
            @display("p=626.56,140.8");
        }
        cp: CloudProvider {
            @display("p=264.704,192.89601");
        }

        dc1: DataCentre {
            @display("p=322.432,156.28801");
        }
        dc2: DataCentre {
            @display("p=380.16,177.408");
        }
        smq: MessageQueue {
            @display("p=125.312004,326.656");
        }
        ug: UserGenerator {
            @display("p=70.4,639.232;is=l");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=626.56,47.872");
        }
        router_bb: Router {
            @display("p=302.72,461.824");
        }
        router1: Router {
            @display("p=343.552,291.456");
        }
        router2: Router {
            @display("p=168.96,409.72803");
        }
        router3: Router {
            @display("p=159.104,615.296");
        }
    connections:
        cp.ethg++ <--> Eth10M <--> router1.ethg++;
        dc1.comm++ <--> Eth10M <--> router1.ethg++;
        dc2.comm++ <--> Eth10M <--> router1.ethg++;

        smq.ethg++ <--> Eth10M <--> router2.ethg++;
        ug.comm <--> Eth10M <--> router3.ethg++;

        router1.ethg++ <--> Eth100G <--> router_bb.ethg++;
        router2.ethg++ <--> Eth100G <--> router_bb.ethg++;
        router3.ethg++ <--> Eth100G <--> router_bb.ethg++;
}

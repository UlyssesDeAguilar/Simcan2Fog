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

// Mission critical modules
import simcan2.Core.SimData.SimData;
import simcan2.Architecture.Network.MessageQueue.MessageQueue;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

// Entities involved in action
import simcan2.Management.CloudProvider.CloudProvider;
import simcan2.Management.DataCentre.DataCentre;
import simcan2.Management.UserGenerators.UserGenerator;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network FogShowcase
{
    @display("bgb=700,700;bgi=maps/europe,c;bgg=100;bgu=km");
    submodules:
        simData: SimData {

            @display("p=626.56,140.8");
        }
        cp: CloudProvider {
            @display("p=328.064,542.08");
        }
        dc: DataCentre {
            @display("p=361,265");
        }
        smq: MessageQueue {
            @display("p=146.432,367.488");
        }
        ug: UserGenerator {
            @display("p=70.4,639.232;is=l");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=626.56,47.872");
        }
        router_bb: Router {
            @display("p=314,401");
        }
    connections:
        cp.ethg++ <--> Eth10M <--> router_bb.ethg++;
        smq.ethg++ <--> Eth10M <--> router_bb.ethg++;
        dc.comm++ <--> Eth10M <--> router_bb.ethg++;
        ug.comm <--> Eth10M <--> router_bb.ethg++;
}

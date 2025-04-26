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

package s2f.simulations.FogShowcase;

// Mission critical modules
import s2f.core.simdata.SimData;
import s2f.architecture.messagequeue.MessageQueue;
import inet.node.inet.Router;
import inet.node.ethernet.EthernetSwitch;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100G;

// Entities involved in action
import s2f.management.cloudprovider.CloudProvider;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;
import s2f.users.generators.UserGenerator;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network FogShowcase2
{
    @display("bgb=700,700;bgi=maps/europe,c;bgg=100;bgu=km");
    types:
    submodules:
        simData: SimData {
            @display("p=626.56,140.8");
        }
        cp: CloudProvider {
            @display("p=283.008,168.96");
        }

        dc1: DataCentre {
            @display("p=366.08002,171.776");
        }

        fg1: FogNode {
            @display("p=185.856,439.29602");
        }

        smq: MessageQueue {
            @display("p=125.312004,326.656");
        }
        ug: UserGenerator {
            @display("p=70.4,639.232;is=l");
        }

        switch: EthernetSwitch {

            @display("p=315.392,247.80801");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=626.56,47.872");
        }
        router1: Router {
            @display("p=308.35202,335.104");
        }
        fg2: FogNode {
            @display("p=260.48,535.04004");
        }
    connections:
        switch.ethg++ <--> Eth10M <--> router1.ethg++;
        dc1.comm++ <--> Eth10M <--> switch.ethg++;
        cp.ethg++ <--> Eth10M <--> switch.ethg++;

        fg1.comm++ <--> Eth100G <--> router1.ethg++;
        fg2.comm++ <--> Eth100G <--> router1.ethg++;
        fg1.comm++ <--> Eth100G <--> fg2.comm++;
        fg1.comm++ <--> Eth100G <--> smq.ethg++;
        ug.comm <--> Eth10M <--> fg2.comm++;

}

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

package s2f.simulations.EdgeMediumScenario;

import s2f.core.simdata.SimData;
import s2f.architecture.dns.DnsSimplified;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import s2f.architecture.messagequeue.MessageQueue;
import s2f.tiles.ITile;

import s2f.management.cloudprovider.CloudProvider;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;

import inet.node.internetcloud.InternetCloud;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.EthernetSwitch;

network MediumNet
{
    parameters:
        @display("bgb=892.44,398.98502;bgg=50");
        int numUsers = 10;

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=698.88,78.4");
        }
        simData: SimData {
            @display("p=781.76,78.4");
        }
        dns: DnsSimplified {
            @display("p=756,188.16");
        }
        dc: DataCentre {
            @display("p=768.32,308");
        }
        fg1: FogNode {
            @display("p=367.36002,327.04");
        }
        fg2: FogNode {
            @display("p=367.36002,277.76");
        }
        fg3: FogNode {
            @display("p=367.36002,219.52");
        }
        fg4: FogNode {
            @display("p=367.36002,170.24");
        }

        users[numUsers]: <> like ITile {
            @display("p=127.68,258.72");
        }
        local: InternetCloud {
            @display("p=237.18001,259.29");
        }
        metropolitan: InternetCloud {
            @display("p=509.6,258.72");
        }

        backbone: InternetCloud {
            @display("p=603.68,258.72");
        }

        cloudProvider: CloudProvider {
            @display("p=657.44,188.16");
        }
        messageQueue: MessageQueue {
            @display("p=657.44,327.04");
        }

        sw1: EthernetSwitch {
            @display("p=184.8,257.6");
        }
        sw2: EthernetSwitch {

            @display("p=297.92,257.6");
        }

        sw3: EthernetSwitch {

            @display("p=449.12,250.88");
        }
        sw4: EthernetSwitch {
            @display("p=698.88,260.96");
        }
    connections:

        for i=0..sizeof(users)-1 {
            users[i].ethernet <--> Eth10G <--> sw1.ethg++;
        }

        local.ethg++ <--> Eth10G <--> sw1.ethg++;
        local.ethg++ <--> Eth10G <--> sw2.ethg++;

        sw2.ethg++ <--> Eth10G <--> fg1.comm++;
        sw2.ethg++ <--> Eth10G <--> fg2.comm++;
        sw2.ethg++ <--> Eth10G <--> fg3.comm++;
        sw2.ethg++ <--> Eth10G <--> fg4.comm++;

        sw3.ethg++ <--> Eth10G <--> fg1.comm++;
        sw3.ethg++ <--> Eth10G <--> fg2.comm++;
        sw3.ethg++ <--> Eth10G <--> fg3.comm++;
        sw3.ethg++ <--> Eth10G <--> fg4.comm++;

        metropolitan.ethg++ <--> Eth10G <--> sw3.ethg++;
        metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;

        sw4.ethg++ <--> Eth100G <--> dc.comm++;
        sw4.ethg++ <--> Eth100G <--> dns.ethg++;
        sw4.ethg++ <--> Eth100G <--> messageQueue.ethg++;
        sw4.ethg++ <--> Eth100G <--> cloudProvider.ethg++;
        sw4.ethg++ <--> Eth100G <--> backbone.ethg++;
}

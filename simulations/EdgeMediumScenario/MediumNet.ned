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

package simcan2.simulations.EdgeMediumScenario;
import simcan2.Core.SimData.SimData;
import simcan2.Architecture.Network.DNS.DnsSimplified;
import simcan2.Management.DataCentre.DataCentre;
import simcan2.Architecture.Nodes.Fog.FogNode;
import simcan2.Architecture.Nodes.Iot.*;
import simcan2.UserModel.Edge.*;
import simcan2.UserModel.Edge.ITile;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.internetcloud.InternetCloud;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.WirelessHost;
import simcan2.Architecture.Nodes.Iot.HumiditySensor;
import simcan2.Architecture.Nodes.Iot.Raspberry5;

network MediumNet
{
    parameters:
        @display("bgb=892.44,398.98502;bgg=50");
        int numUsers = 10;

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=626.115,79.395004");
        }
        simData: SimData {
            @display("p=711.54004,79.395004");
        }
        dns: DnsSimplified {
            @display("p=720.585,188.94");
        }
        dc: DataCentre {
            @display("p=719.58,293.46");
        }
        fg1: FogNode {
            @display("p=359.79,338.685");
        }
        fg2: FogNode {
            @display("p=358.785,278.385");
        }
        fg3: FogNode {
            @display("p=358.785,214.065");
        }
        fg4: FogNode {
            @display("p=358.785,157.785");
        }

        users[numUsers]: <> like ITile {
            @display("p=137.685,261.30002");
        }
        local: InternetCloud {
            @display("p=237.18001,259.29");
        }
        metropolitan: InternetCloud {
            @display("p=481.39502,262.305");
        }

        backbone: InternetCloud {
            @display("p=618.075,261.30002");
        }
    connections:

        for i=0..sizeof(users)-1 {
            users[i].ethernet <--> Eth10G <--> local.ethg++;
        }

        local.ethg++ <--> Eth10G <--> fg1.comm++;
        local.ethg++ <--> Eth10G <--> fg2.comm++;
        local.ethg++ <--> Eth10G <--> fg3.comm++;
        local.ethg++ <--> Eth10G <--> fg4.comm++;

        metropolitan.ethg++ <--> Eth10G <--> fg1.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg2.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg3.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg4.comm++;

        metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;
        backbone.ethg++ <--> Eth100G <--> dc.comm++;
        backbone.ethg++ <--> Eth100G <--> dns.ethg++;
}

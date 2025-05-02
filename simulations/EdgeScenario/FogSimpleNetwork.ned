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

import s2f.core.simdata.SimData;
import s2f.architecture.dns.DnsSimplified;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import s2f.architecture.messagequeue.MessageQueue;

import s2f.management.cloudprovider.CloudProvider;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;

import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.EthernetSwitch;
import inet.node.internetcloud.InternetCloud;

import s2f.users.edge.EdgeUser;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.nodes.iot.sensors.HumiditySensor;
import s2f.architecture.nodes.iot.actuators.AutomaticWatering;
import s2f.architecture.nodes.iot.controllers.Raspberry5;

network SimpleFogNetwork
{
    parameters:
        int edgeTiles = default(1);
        string serviceName = default("service");

        @display("bgb=800,400;bgg=50");
    submodules:
        simData: SimData {

            @display("p=752.745,27.135");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=675.36,27.135");
        }

        cloudProvider: CloudProvider {

            @display("p=483,308");
        }
        messageQueue: MessageQueue {

            @display("p=483,173");
        }
        user: EdgeUser {
            @display("p=180,46");
        }

        netStack: NetworkStack {
            @display("p=180.90001,124.62");
        }
        dns: DnsSimplified {
            @display("p=613.05,173.865");
        }

        dc: DataCentre {
            @display("p=613.05,295.47");
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
            @display("p=500.49002,248.235");
        }
        sw: EthernetSwitch {

            @display("p=555.765,246.225");
        }
        sensor: HumiditySensor {
            numWlanInterfaces = 0;
            @display("p=105.525,248.235");
        }
        controller: Raspberry5 {

            @display("p=84.42,87.435005");
        }
        ap: EthernetSwitch {
            @display("i=device/accesspoint, p=144.72,199.99501");
        }
        actuator[1]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("i=faucet64;p=42.21,189.945");
        }
    connections:

        user.serialOut --> controller.serialIn;
        user.serialIn <-- controller.serialOut;
        user.queueIn <-- netStack.queueOut;
        user.queueOut --> netStack.queueIn;

        sensor.ethg++ <--> Eth100M <--> ap.ethg++;
        controller.ethg++ <--> Eth100M <--> ap.ethg++;
        netStack.ethg++ <--> Eth100M <--> ap.ethg++;
        actuator[0].ethg++ <--> Eth100M <--> ap.ethg++;

        ap.ethg++ <--> Eth10G <--> local.ethg++;
        local.ethg++ <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth10G <--> fg.comm++;
        metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;

        backbone.ethg++ <--> Eth100G <--> sw.ethg++;
        sw.ethg++ <--> Eth100G <--> dc.comm++;
        sw.ethg++ <--> Eth100G <--> dns.ethg++;
        sw.ethg++ <--> Eth100G <--> cloudProvider.ethg++;
        sw.ethg++ <--> Eth100G <--> messageQueue.ethg++;
}

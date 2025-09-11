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

package s2f.simulations.P2PShowcase;

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
import ned.IdealChannel;
import inet.node.ethernet.Eth10G;

import s2f.users.edge.EdgeUser;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.nodes.iot.sensors.HumiditySensor;
import s2f.architecture.nodes.iot.actuators.AutomaticWatering;
import s2f.architecture.nodes.iot.controllers.Raspberry5;

module P2PUser
{
    parameters:
        @display("i=abstract/person,,0;bgb=,,,,0");
        networkStack.nodeTopic = fullName();
    gates:
        inout ethg;
    submodules:
        edgeUser: EdgeUser {
            @display("p=64,98");
        }
        networkStack: NetworkStack {
            @display("p=188,98");
        }
        controller: Raspberry5 {
            @display("p=84.42,87.435005");
        }
    connections allowunconnected:
        edgeUser.serialOut --> controller.serialIn;
        edgeUser.serialIn <-- controller.serialOut;
        networkStack.queueOut --> IdealChannel --> edgeUser.queueIn;
        edgeUser.queueOut --> IdealChannel --> networkStack.queueIn;

        networkStack.ethg++ <--> IdealChannel <--> ethg;
}





network SimpleP2PNetwork
{
    parameters:
        int edgeTiles = default(1);

        @display("bgb=411,344;bgg=50");

    submodules:
        simData: SimData {

            @display("p=372,27");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=304,27");
        }
        messageQueue: MessageQueue {

            @display("p=171,254");
        }
        userA: P2PUser {
            @display("p=73.365,173.865");
        }
        userB: P2PUser {
            @display("p=270.345,173.865");
        }
        dns: DnsSimplified {
            @display("p=171.855,78.39");
        }

        sw: EthernetSwitch {

            @display("p=171.855,172.86");
        }

    connections:
        userA.ethg <--> Eth10G <--> sw.ethg++;
        userB.ethg <--> Eth10G <--> sw.ethg++;
        sw.ethg++ <--> Eth10G <--> dns.ethg++;
        sw.ethg++ <--> Eth100G <--> messageQueue.ethg++;
}

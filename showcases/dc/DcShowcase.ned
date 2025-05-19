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

package s2f.showcases.dc;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EthernetSwitch;
import ned.IdealChannel;
import s2f.architecture.dns.DnsSimplified;
import s2f.architecture.dns.resolver.DnsResolverServer;
import s2f.architecture.messagequeue.MessageQueue;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.core.simdata.SimData;
import s2f.management.cloudprovider.CloudProvider;
import s2f.users.edge.EdgeUser;
import s2f.architecture.nodes.iot.controllers.Raspberry5;

module DcUser
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
    connections allowunconnected:
        networkStack.queueOut --> IdealChannel --> edgeUser.queueIn;
        edgeUser.queueOut --> IdealChannel --> networkStack.queueIn;


        networkStack.ethg++ <--> IdealChannel <--> ethg;
}

module DcEdgeUser
{
    parameters:
        @display("i=abstract/person,,0;bgb=,,,,0");
        networkStack.nodeTopic = fullName();
    gates:
        inout ethg[];
    submodules:
        edgeUser: EdgeUser {
            @display("p=146,141");
        }
        networkStack: NetworkStack {
            @display("p=221,53");
        }

        raspberry5: Raspberry5 {
            @display("p=83,53");
        }
    connections:
        networkStack.queueOut --> IdealChannel --> edgeUser.queueIn;
        edgeUser.queueOut --> IdealChannel --> networkStack.queueIn;
        raspberry5.ethg++ <--> ethg++;
        networkStack.ethg++ <--> ethg++;
        edgeUser.serialOut --> IdealChannel --> raspberry5.serialIn;
        raspberry5.serialOut --> IdealChannel --> edgeUser.serialIn;
}

network DcShowcase
{
    parameters:
        int numDcUsers = default(1);

    submodules:
        dataCentre: DataCentre {
            @display("p=361,213");
        }
        messageQueue: MessageQueue {
            @display("p=151,122");
        }
        cloudProvider: CloudProvider {
            @display("p=284,122");
        }
        ethernetSwitch: EthernetSwitch {
            @display("p=210,212");
        }
        simData: SimData {
            @display("p=33,26");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=151,26");
        }
        dns: DnsSimplified {
            @display("p=275,297");
        }
        dnsResolverServer: DnsResolverServer {
            @display("p=151,297");
        }
        dcUsers[numDcUsers]: DcUser {
            @display("p=82,213");
        }
    connections:
        ethernetSwitch.ethg++ <--> Eth10M <--> messageQueue.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> cloudProvider.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dataCentre.comm++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dnsResolverServer.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dns.ethg++;

        for i=0..numDcUsers-1 {
            dcUsers[i].ethg <--> Eth10M <--> ethernetSwitch.ethg++;
        }
}

network EdgeDcShowcase
{
    parameters:
        int numDcUsers = default(1);

    submodules:
        dataCentre: DataCentre {
            @display("p=361,213");
        }
        messageQueue: MessageQueue {
            @display("p=151,122");
        }
        cloudProvider: CloudProvider {
            @display("p=284,122");
        }
        ethernetSwitch: EthernetSwitch {
            @display("p=210,212");
        }
        simData: SimData {
            @display("p=33,26");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=151,26");
        }
        dns: DnsSimplified {
            @display("p=275,297");
        }
        dnsResolverServer: DnsResolverServer {
            @display("p=151,297");
        }
        dcUsers[numDcUsers]: DcEdgeUser {
            @display("p=82,213");
        }
    connections:
        ethernetSwitch.ethg++ <--> Eth10M <--> messageQueue.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> cloudProvider.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dataCentre.comm++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dnsResolverServer.ethg++;
        ethernetSwitch.ethg++ <--> Eth10M <--> dns.ethg++;

        for i=0..numDcUsers-1, for j=0..1 {
            dcUsers[i].ethg++ <--> Eth10M <--> ethernetSwitch.ethg++;
        }
}

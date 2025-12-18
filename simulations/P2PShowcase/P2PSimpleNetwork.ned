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

import s2f.apps.p2p.P2Protocol;
import s2f.apps.p2p.discovery.dns.DnsDiscoveryService;
import s2f.architecture.net.stack.proxy.AppProxy;
import s2f.architecture.net.stack.proxy.ServiceTable;
import s2f.architecture.net.stack.resolver.DnsResolver;
import s2f.architecture.dns.resolver.DnsResolverServer;
import s2f.architecture.dns.DnsSimplified;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.StandardHost;

module PeerNode extends StandardHost
{
    submodules:
        protocol: P2Protocol {
            @display("p=932.554,60.966003");
        }

        resolver: DnsResolver {
            @display("p=1106.42,60.966003");
        }
    connections allowunconnected:
        // Protocol connects to the TCP/UDP Layer to handle peers
        at.in++ <-- protocol.socketOut;
        at.out++ --> protocol.socketIn;

        // Resolver connects to the TCP/UDP layer
        resolver.socketOut --> at.in++;
        resolver.socketIn <-- at.out++;

        // Discovery services
        //at.in++ <-- protocol.serviceSocketOut++;
        //at.out++ --> protocol.serviceSocketIn++;
}

//
// Network that showcases p2p peer discovery and file sharing
//
network P2PNetwork
{
    parameters:
        int numPeers = default(2);

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=67,36");
        }
        switch: EthernetSwitch {
            @display("p=181,137");
        }
        peers[numPeers]: PeerNode;
        dns: DnsSimplified {
            @display("p=246,87");
        }
    connections allowunconnected:
        for i=0..numPeers-1 {
            peers[i].ethg++ <--> Eth10M <--> switch.ethg++;
        }
        //peerD.ethg++ <--> Eth10M <--> switch.ethg++;
        //peerE.ethg++ <--> Eth10M <--> switch.ethg++;
        dns.ethg++ <--> Eth10M <--> switch.ethg++;
}

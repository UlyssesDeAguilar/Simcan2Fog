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

package s2f.Architecture.Network.Stack;
import s2f.Architecture.Network.Stack.StackMultiplexer;
import s2f.Architecture.Network.Stack.Queue.QueuePullClient;
import s2f.Architecture.Network.Stack.Queue.QueuePushClient;
import s2f.Architecture.Network.Stack.Resolver.DnsResolver;
import s2f.Architecture.Network.DNS.DnsService;
import inet.applications.tcpapp.TcpClientSocketIo;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.Router;

module RoutingNetworkStack extends Router
{
    parameters:
        string nodeTopic = default(""); // The topic that will be used in the Message Queue (must be unique), leave empty for auto assignment
        string cpIp = default("10.0.1.1");
        string smqIp = default("10.0.2.1");
        bool hasCloudEvents = default(false);	// Wheter the module sends events to the cloud provider
        bool enableQueue = default(true);		// Wheter to instatiate or not the queue push/pull clients
        string dns = default("10.1.0.1");
        string tld = default("com");
        @dnsTld(tld);
		dnsResolver.ispResolver = dns;
    gates:
        input queueIn @loose;
        output queueOut @loose;
        input eventsIn @loose;	// If hasCloudEvents is true this should be connected!
        inout eth_comm;
		input ssIn[];
		output ssOut[];
		
    submodules:
        sm: StackMultiplexer {
            @display("p=1187.5499,66.15");
        }

        dnsResolver: DnsResolver {
            @display("p=1084.98,202.79999");
        }

        dnsServer: DnsService {
            mode = "ns";
            serverName = "";
            @display("p=1090.0499,74.36");
        }

        pushSocket: TcpClientSocketIo if enableQueue {
            connectAddress = smqIp;
            connectPort = 8444;
            @display("p=748,70");
        }

        pullSocket: TcpClientSocketIo if enableQueue {
            connectAddress = smqIp;
            connectPort = 8443;
            @display("p=883,69");
        }

        pullClient: QueuePullClient if enableQueue {
            @display("p=960.27747,69.615");
        }

        pushClient: QueuePushClient if enableQueue {
            @display("p=662,71");
        }

        eventsSocket: UdpSocketIo if hasCloudEvents {
            destAddress = cpIp;
            destPort = 8446;
            @display("p=579,70");
        }

    connections allowunconnected:
        // Bidirectional ethernet comms
        eth_comm <--> sm.comm;

        dnsResolver.socketIn <-- at.out++;
        dnsResolver.socketOut --> at.in++;

        dnsServer.socketIn <-- at.out++;
        dnsServer.socketOut --> at.in++;

        // Connect sockets to transport layer
        pullSocket.socketIn <-- at.out++ if enableQueue;
        pullSocket.socketOut --> at.in++ if enableQueue;

        pushSocket.socketIn <-- at.out++ if enableQueue;
        pushSocket.socketOut --> at.in++ if enableQueue;

        eventsSocket.socketIn <-- at.out++ if hasCloudEvents;
        eventsSocket.socketOut --> at.in++ if hasCloudEvents;

        // Connect clients to their sockets
        pullClient.netOut --> pullSocket.trafficIn if enableQueue;
        pullClient.netIn <-- pullSocket.trafficOut if enableQueue;

        pushClient.netOut --> pushSocket.trafficIn if enableQueue;
        pushClient.netIn <-- pushSocket.trafficOut if enableQueue;
		
		for i=0..sizeof(ssIn)-1 {
		    ssIn[i] --> at.in++;
		    ssOut[i] <-- at.out++;
		}
		
        // Incoming and outgoing from SMQ
        pullClient.queueOut --> queueOut if enableQueue;
        queueIn --> pushClient.queueIn if enableQueue;
		
        // Outgoing to the cloud provider
        eventsIn --> eventsSocket.trafficIn if hasCloudEvents;
}

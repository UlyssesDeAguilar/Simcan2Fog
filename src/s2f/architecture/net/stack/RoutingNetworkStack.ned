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
package s2f.architecture.net.stack;

import s2f.architecture.net.stack.resolver.DnsResolver;
import s2f.architecture.net.stack.queue.QueuePullClient;
import s2f.architecture.net.stack.queue.QueuePushClient;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.StandardHost;
import inet.node.inet.Router;

module RoutingNetworkStack extends Router
{
    parameters:
        string nodeTopic; 						// The topic that will be used in the Message Queue (must be unique), leave empty for auto assignment
        string smqIp = default("10.0.2.1");
        bool enableQueue = default(true);		// Wheter to instatiate or not the queue push/pull clients
        string dns = default("10.1.0.1");
		dnsResolver.ispResolver = dns;
    gates:
        input queueIn @loose;
        output queueOut @loose;
		input ssIn[];
		output ssOut[];
		
    submodules:
        dnsResolver: DnsResolver {
            @display("p=1140.396,65.598");
        }

        pushClient: QueuePushClient if enableQueue {
            @display("p=857.81995,68.962");
            connectAddress = smqIp;
        }

        pullClient: QueuePullClient if enableQueue {
            @display("p=973.878,75.689995");
            connectAddress = smqIp;
        }

        eventsSocket: UdpSocketIo if hasCloudEvents {
            destAddress = cpIp;
            destPort = 8446;
            @display("p=579,70");
        }

    connections:
		// Connect dns resolver
        dnsResolver.socketIn <-- at.out++;
        dnsResolver.socketOut --> at.in++;

        // Connect both push/pull clients if enabled
        pullClient.socketIn <-- at.out++ if enableQueue;
        pullClient.socketOut --> at.in++ if enableQueue;

        pushClient.socketIn <-- at.out++ if enableQueue;
        pushClient.socketOut --> at.in++ if enableQueue;

        // Interconnect possible apps to the tcp/udp layer
        for i=0..sizeof(ssIn)-1 {
            ssIn[i] --> at.in++;
            ssOut[i] <-- at.out++;
        }

		// Incoming and outgoing from SMQ
        pullClient.queueOut --> queueOut if enableQueue;
        queueIn --> pushClient.queueIn if enableQueue;
}

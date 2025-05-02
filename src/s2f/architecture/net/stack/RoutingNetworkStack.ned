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
import s2f.architecture.net.stack.queue.QueueClient;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.StandardHost;
import inet.node.inet.Router;

module RoutingNetworkStack extends Router
{
    parameters:
        string nodeTopic; 						// The topic that will be used in the Message Queue (must be unique), leave empty for auto assignment
        string smqIp;
        string dns;
        bool enableQueue = default(true);		// Wheter to instatiate or not the queue push/pull clients

        dnsResolver.ispResolver = dns;
        queueClient.topic = nodeTopic;
        queueClient.connectAddress = smqIp;
        @display("bgb=1542.214,1124.484");
    gates:
        input queueIn @loose;
        output queueOut @loose;
        input ssIn[];
        output ssOut[];

    submodules:
        dnsResolver: DnsResolver {
            @display("p=672.88403,74.514");
        }

        queueClient: QueueClient if enableQueue {
            @display("p=1357.058,97.094");
        }

    connections:
		// Connect dns resolver
        dnsResolver.socketIn <-- at.out++;
        dnsResolver.socketOut --> at.in++;

        // Connect both push/pull clients if enabled
        queueClient.socketIn <-- at.out++ if enableQueue;
        queueClient.socketOut --> at.in++ if enableQueue;

        // Interconnect possible apps to the tcp/udp layer
        for i=0..sizeof(ssIn)-1 {
            ssIn[i] --> at.in++;
            ssOut[i] <-- at.out++;
        }

		// Incoming and outgoing from SMQ
        queueClient.queueOut --> queueOut if enableQueue;
        queueClient.queueIn <-- queueIn if enableQueue;
}

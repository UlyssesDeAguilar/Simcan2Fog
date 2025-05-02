package s2f.architecture.net.stack;

import s2f.architecture.net.stack.resolver.DnsResolver;
import s2f.architecture.net.stack.queue.QueueClient;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.StandardHost;

// This is the main network technology stack for hypervisors.
//
// It contains an DNS Resolver, Queue Pull Client, Queue Push Client
//
// They work closely with the hypervisor in order to provide and consume services
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-05-05
//
module NetworkStack extends StandardHost
{
    parameters:
        @display("i=device/card;bgb=2287.1833,1180.7639");     	// We have to make an icon (default is a PC)
        string nodeTopic; 										// The topic that will be used in the Message Queue (must be unique) 
        string smqIp;
        string dns;
        bool enableQueue = default(true);		// Whether to instatiate or not the queue push/pull clients

        dnsResolver.ispResolver = dns;
        queueClient.topic = nodeTopic;
        queueClient.connectAddress = smqIp;
    gates:
        input queueIn @loose;
        output queueOut @loose;

        input ssIn[];
        output ssOut[];

    submodules:
        dnsResolver: DnsResolver {
            @display("p=973.675,74.4575");
        }

        queueClient: QueueClient if enableQueue {
            @display("p=1219.9575,74.4575");
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

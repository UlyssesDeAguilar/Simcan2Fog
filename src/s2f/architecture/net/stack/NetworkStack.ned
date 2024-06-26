package s2f.architecture.net.stack;

import s2f.architecture.net.stack.resolver.DnsResolver;
import s2f.architecture.net.stack.queue.QueuePullClient;
import s2f.architecture.net.stack.queue.QueuePushClient;
import inet.applications.tcpapp.TcpClientSocketIo;
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
        @display("i=device/card;bgb=1255.1174,837.4274");     	// We have to make an icon (default is a PC)
        string nodeTopic = default(""); // The topic that will be used in the Message Queue (must be unique), leave empty for auto assignment  

        string cpIp = default("10.0.1.1");
        string smqIp = default("10.0.2.1");
        string dns = default("10.1.0.1");

        bool hasCloudEvents = default(false);	// Whether the module sends events to the cloud provider
        bool enableQueue = default(true);		// Whether to instatiate or not the queue push/pull clients

        dnsResolver.ispResolver = dns;
    gates:
        input queueIn @loose;
        output queueOut @loose;
        input eventsIn @loose;	// If hasCloudEvents is true this should be connected!

        input ssIn[];
        output ssOut[];

    submodules:
        dnsResolver: DnsResolver {
            @display("p=1100.028,75.689995");
        }

        pushClient: QueuePushClient if enableQueue {
            @display("p=748,70");
            connectAddress = smqIp;
        }

        pullClient: QueuePullClient if enableQueue {
            @display("p=960.27747,69.615");
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

        eventsSocket.socketIn <-- at.out++ if hasCloudEvents;
        eventsSocket.socketOut --> at.in++ if hasCloudEvents;

        // Interconnect possible apps to the tcp/udp layer
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

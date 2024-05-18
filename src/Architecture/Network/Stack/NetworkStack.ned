package simcan2.Architecture.Network.Stack;

import simcan2.Architecture.Network.Stack.StackMultiplexer;
import simcan2.Architecture.Network.Stack.Queue.QueuePullClient;
import simcan2.Architecture.Network.Stack.Queue.QueuePushClient;
import inet.applications.tcpapp.TcpClientSocketIo;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.StandardHost;

// This is the main network technology stack for hypervisors.
//
// It contains an HttpClient, HttpProxy, DNS Resolver, DNS Local Server, Queue Pull Client, Queue Push Client
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
        bool hasCloudEvents = default(false);	// Wheter the module sends events to the cloud provider
        numApps = 4;
        app[0].typename = "HttpClientService";
        app[1].typename = "HttpProxyService";
        app[2].typename = "DNS_Resolver";
        app[3].typename = "DNS_Service";
    gates:
        input queueIn;
        output queueOut;
        inout eth_comm;
        input eventsIn @loose;	// If hasCloudEvents is true this should be connected!

    submodules:
        sm: StackMultiplexer {
            @display("p=1187.5499,66.15");
        }

        pushSocket: TcpClientSocketIo {
            connectAddress = "24.24.24.24";
            connectPort = 8444;
            @display("p=748,70");
        }

        pullSocket: TcpClientSocketIo {
            connectAddress = "24.24.24.24";
            connectPort = 8443;
            @display("p=883,69");
        }

        pullClient: QueuePullClient {
            @display("p=960.27747,69.615");
        }

        pushClient: QueuePushClient {
            @display("p=662,71");
        }

        eventsSocket: UdpSocketIo if hasCloudEvents {
            destAddress = "1.1.1.1";
            destPort = 8446;
            @display("p=579,70");
        }

    connections allowunconnected:
        // Bidirectional ethernet comms
        eth_comm <--> sm.comm;

        // Connect sockets to transport layer
        pullSocket.socketIn <-- at.out++;
        pullSocket.socketOut --> at.in++;

        pushSocket.socketIn <-- at.out++;
        pushSocket.socketOut --> at.in++;

        eventsSocket.socketIn <-- at.out++ if hasCloudEvents;
        eventsSocket.socketOut --> at.in++ if hasCloudEvents;

        // Connect clients to their sockets
        pullClient.netOut --> pullSocket.trafficIn;
        pullClient.netIn <-- pullSocket.trafficOut;

        pushClient.netOut --> pushSocket.trafficIn;
        pushClient.netIn <-- pushSocket.trafficOut;

        // Incoming and outgoing from SMQ
        pullClient.queueOut --> queueOut;
        queueIn --> pushClient.queueIn;

        // Outgoing to the cloud provider
        eventsIn --> eventsSocket.trafficIn;
}

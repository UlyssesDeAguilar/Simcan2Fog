package simcan2.Management.CloudProvider;

import simcan2.Architecture.Network.Stack.Queue.QueuePullClient;
import simcan2.Architecture.Network.Stack.Queue.QueuePushClient;
import inet.applications.tcpapp.TcpClientSocketIo;
import inet.applications.udpapp.UdpSocketIo;
import inet.node.inet.StandardHost;

// Handles the dispatching of user requests
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-05-17
//
module CloudProvider extends StandardHost
{
    parameters:
        string nodeTopic = default("CloudProvider"); // The topic that will be used in the Message Queue (must be unique), leave empty for auto assignment  

        @display("bgb=1308.825,883.57495;i=cloud-manager");
    submodules:
        cpService: CloudProviderService {
            @display("p=998,33");
        }

        listenerSocket: UdpSocketIo {
            @display("p=997,93");
            localPort = 8446;
        }

        pushSocket: TcpClientSocketIo {
            connectAddress = "24.24.24.24";
            connectPort = 8444;
            @display("p=687,88");
        }

        pullSocket: TcpClientSocketIo {
            connectAddress = "24.24.24.24";
            connectPort = 8443;
            @display("p=783,88");
        }

        pullClient: QueuePullClient {
            @display("p=851,87");
        }

        pushClient: QueuePushClient {
            @display("p=630,30");
        }

    connections allowunconnected:
        // Connect sockets to transport layer
        pullSocket.socketIn <-- at.out++;
        pullSocket.socketOut --> at.in++;

        pushSocket.socketIn <-- at.out++;
        pushSocket.socketOut --> at.in++;

        listenerSocket.socketIn <-- at.out++;
        listenerSocket.socketOut --> at.in++;

        // Connect clients to their sockets
        pullClient.netOut --> pullSocket.trafficIn;
        pullClient.netIn <-- pullSocket.trafficOut;

        pushClient.netOut --> pushSocket.trafficIn;
        pushClient.netIn <-- pushSocket.trafficOut;

        // Incoming and outgoing from SMQ
        pullClient.queueOut --> cpService.queueIn;
        cpService.queueOut --> pushClient.queueIn;

        // Incoming updates from nodes
        listenerSocket.socketOut --> cpService.listener;
}


package s2f.management.cloudprovider;

import s2f.architecture.net.stack.queue.QueuePullClient;
import s2f.architecture.net.stack.queue.QueuePushClient;
import s2f.architecture.net.stack.listener.UdpListener;
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
        string smqIp = default("10.0.2.1");
        @display("bgb=1308.825,883.57495;i=cloud-manager");
    submodules:
        cpService: CloudProviderService {
            @display("p=998,33");
        }

        listenerSocket: UdpListener {
            @display("p=997,93");
            localPort = 8446;
        }

        pushClient: QueuePushClient {
            connectAddress = smqIp;
            @display("p=687,88");
        }

        pullClient: QueuePullClient {
            connectAddress = smqIp;
            @display("p=783,88");
        }

    connections:
        // Connect sockets to transport layer
        pullClient.socketIn <-- at.out++;
        pullClient.socketOut --> at.in++;

        pushClient.socketIn <-- at.out++;
        pushClient.socketOut --> at.in++;

        listenerSocket.socketIn <-- at.out++;
        listenerSocket.socketOut --> at.in++;

        // Incoming and outgoing from SMQ
        pullClient.queueOut --> cpService.queueIn;
        cpService.queueOut --> pushClient.queueIn;

        // Incoming updates from nodes
        listenerSocket.trafficOut --> cpService.listener;
}


package s2f.management.cloudprovider;

import s2f.architecture.net.stack.queue.QueueClient;
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

        cpService.nodeDbPath = "^.nodeDb";
        queueClient.connectAddress = smqIp;
        queueClient.topic = fullName();
        @display("bgb=1308.825,883.57495;i=cloud-manager");

    submodules:
        cpService: CloudProviderService {
            @display("p=993.43994,65.638");
        }

        queueClient: QueueClient {
            @display("p=830.232,65.638");
        }

        nodeDb: NodeDb {

            @display("p=1177.9359,65.638");
        }
    connections:
        // Connect sockets to transport layer
        queueClient.socketIn <-- at.out++;
        queueClient.socketOut --> at.in++;

        // Incoming and outgoing from SMQ
        queueClient.queueOut --> cpService.queueIn;
        cpService.queueOut --> queueClient.queueIn;
}


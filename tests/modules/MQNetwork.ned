import simcan2.Architecture.Network.Stack.NetworkStack;
import simcan2.Architecture.Network.MessageQueue.MessageQueue;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

import mq.MQDriver;

network MQNetwork {
    parameters:
        string endPointTopic = default("endPointTopic");

    submodules:
        queue: MessageQueue;

        client: MQDriver {
            kickStart = true;
        }

        server: MQDriver;

        ns1: NetworkStack;
        ns2: NetworkStack{
            nodeTopic = endPointTopic;
        }

        router: Router;
        configurator: Ipv4NetworkConfigurator;
    
    connections allowunconnected:
        client.in <-- ns1.queueOut;
        client.out --> ns1.queueIn;
        
        server.in <-- ns2.queueOut;
        server.out --> ns2.queueIn;

        ns1.ethg++ <--> Eth10M <--> router.ethg++;
        ns2.ethg++ <--> Eth10M <--> router.ethg++;
        queue.ethg++ <--> Eth10M <--> router.ethg++;
}
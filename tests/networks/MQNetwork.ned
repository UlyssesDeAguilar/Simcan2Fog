package s2f.tests.networks;

import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.messagequeue.MessageQueue;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import s2f.architecture.messagequeue.MessageQueue;
import inet.node.ethernet.EthernetSwitch;
import inet.node.ethernet.Eth10M;

import s2f.tests.modules.mq.MQDriver;

network MQNetwork
{
    parameters:
        string endPointTopic = default("endPointTopic");

        @display("bgb=378,265");
    submodules:
        queue: MessageQueue {
            @display("p=180,78");
        }
        client: MQDriver {
            @display("p=319,143");
        }
        server: MQDriver;
        ns1: NetworkStack {
            @display("p=251,143");
        }
        ns2: NetworkStack {
            @display("p=112,143");
        }
        switch: EthernetSwitch {
            @display("p=180,142");
        }
        configurator: Ipv4NetworkConfigurator {

            @display("p=336,30");
        }
    connections allowunconnected:
        client.in <-- ns1.queueOut;
        client.out --> ns1.queueIn;

        server.in <-- ns2.queueOut;
        server.out --> ns2.queueIn;

        ns1.ethg++ <--> Eth10M <--> switch.ethg++;
        ns2.ethg++ <--> Eth10M <--> switch.ethg++;
        queue.ethg++ <--> Eth10M <--> switch.ethg++;
}

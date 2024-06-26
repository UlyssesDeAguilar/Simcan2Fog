import s2f.Architecture.Network.Stack.NetworkStack;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

import ping.PingClient;
import ping.PingServer;

network SimpleNetworkScenario {
    parameters:
        string serverAddress;
    submodules:
        client: PingClient;
        server: PingServer;

        ns1: NetworkStack;
        ns2: NetworkStack;

        router: Router;
        configurator: Ipv4NetworkConfigurator;
    connections allowunconnected:
        client.comm <--> ns1.eth_comm;
        server.comm <--> ns2.eth_comm;

        ns1.ethg++ <--> Eth10M <--> router.ethg++;
        ns2.ethg++ <--> Eth10M <--> router.ethg++;
}
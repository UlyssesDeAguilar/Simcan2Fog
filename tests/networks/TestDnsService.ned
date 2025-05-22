package s2f.tests.networks;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import s2f.architecture.dns.DnsSimplified;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.dns.client.DnsClientService;
import s2f.tests.modules.dns.DnsTestClient;
import inet.node.ethernet.EthernetSwitch;
import inet.node.ethernet.Eth10M;


network TestDnsNetwork
{
    @display("bgb=575,347");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=252,32");
        }
        dns: DnsSimplified {
            @display("p=471,159");
        }
        switch: EthernetSwitch {
            @display("p=383,158");
        }
        netStack: NetworkStack {
            @display("p=301,159");
        }
        client: DnsClientService {
            @display("p=189,159");
        }
        dnsClient: DnsTestClient {
            @display("p=118,159");
        }
    connections:
        switch.ethg++ <--> Eth10M <--> dns.ethg++;
        switch.ethg++ <--> Eth10M <--> netStack.ethg++;

        client.socketOut --> netStack.ssIn++;
        client.socketIn <-- netStack.ssOut++;

        dnsClient.clientOut --> client.clientIn;
        dnsClient.clientIn <-- client.clientOut;
}

package simcan2.Architecture.Network.DNS;
import inet.node.inet.StandardHost;

module DnsSimplified extends StandardHost
{
    parameters:
        @display("i=dns");
        
    submodules:
        dnsServer: DnsServiceSimplified {
            @display("i=block/table;p=858.375,58.274998");
        }
    connections:
        dnsServer.socketIn <-- at.out++;
        dnsServer.socketOut --> at.in++;
}

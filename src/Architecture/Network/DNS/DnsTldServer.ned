package simcan2.Architecture.Network.DNS;
import inet.node.inet.StandardHost;

module DnsTldServer extends DnsServer
{
    parameters:
        @display("i=dns");
        dnsServer.mode = "tld";
        dnsServer.serverName = default("com");
}

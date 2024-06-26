package s2f.architecture.dns.server;
import inet.node.inet.StandardHost;

module DnsRootServer extends DnsServer
{
    parameters:
        @display("i=dns");
        dnsServer.mode = "root";
        dnsServer.serverName = default("I");
}

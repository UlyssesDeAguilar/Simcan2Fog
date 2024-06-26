package s2f.architecture.dns.server;
import inet.node.inet.StandardHost;

module DnsServer extends StandardHost
{
    parameters:
        @display("i=dns");
        
    submodules:
        dnsServer: DnsService {
            @display("i=block/table;p=858.375,58.274998");
        }
    connections:
        dnsServer.socketIn <-- at.out++;
        dnsServer.socketOut --> at.in++;
}

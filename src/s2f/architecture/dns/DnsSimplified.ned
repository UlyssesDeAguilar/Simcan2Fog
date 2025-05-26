package s2f.architecture.dns;
import inet.node.inet.StandardHost;
import s2f.architecture.dns.db.DnsDb;
import s2f.architecture.dns.registry.DnsRegistryService;
import s2f.architecture.dns.server.DnsServiceSimplified;

//
// Simplified DNS server implementation
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
module DnsSimplified extends StandardHost
{
    parameters:
        @display("i=dns");
        
    submodules:
        dnsService: DnsServiceSimplified {
            @display("i=block/table;p=858.375,58.274998");
        }
        db: DnsDb;
        registryService: DnsRegistryService;
    
    connections:
        registryService.socketIn <-- at.out++;
        registryService.socketOut --> at.in++;

        dnsService.socketIn <-- at.out++;
        dnsService.socketOut --> at.in++;
}

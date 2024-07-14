package s2f.architecture.dns;
import inet.node.inet.StandardHost;
import s2f.architecture.dns.cache.DnsCache;
import s2f.architecture.dns.registry.DnsRegistryService;


//
// Module that simplifies the DNS hierarchy by aggregating 
// all the domains onto a single server
//
module DnsSimplified extends StandardHost
{
    parameters:
        @display("i=dns");
        
    submodules:
        dnsService: DnsServiceSimplified {
            @display("i=block/table;p=858.375,58.274998");
        }
        
        cache: DnsCache;

        registryService: DnsRegistryService;
    connections:
        registryService.socketIn <-- at.out++;
        registryService.socketOut --> at.in++;

        dnsService.socketIn <-- at.out++;
        dnsService.socketOut --> at.in++;
}

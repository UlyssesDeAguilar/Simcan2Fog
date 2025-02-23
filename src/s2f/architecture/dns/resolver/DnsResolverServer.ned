package s2f.architecture.dns.resolver;

import s2f.architecture.dns.db.DnsDb;
import inet.node.inet.StandardHost;

module DnsResolverServer extends StandardHost
{
    parameters:
        @display("i=dns");

    submodules:
        resolver: DnsResolverService {
            @display("i=block/table;p=907.19995,58.274998");
        }
        
        db: DnsDb {
            @display("p=1053.6749,58.274998");
        }
    connections:
        resolver.socketIn <-- at.out++;
        resolver.socketOut --> at.in++;
}

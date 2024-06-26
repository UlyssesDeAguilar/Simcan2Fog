package s2f.architecture.dns.resolver;

import s2f.architecture.dns.cache.DnsCache;
import inet.node.inet.StandardHost;

module DnsResolverServer extends StandardHost
{
    parameters:
        @display("i=dns");

    submodules:
        dnsServer: DnsResolverService {
            @display("i=block/table;p=907.19995,58.274998");
        }
        dnsQuerier: DnsQuerierService {
            @display("p=743.39996,55.125");
        }
        cache: DnsCache {
            @display("p=1053.6749,58.274998");
        }
    connections:
        dnsServer.socketIn <-- at.out++;
        dnsServer.socketOut --> at.in++;

        dnsQuerier.socketIn <-- at.out++;
        dnsQuerier.socketOut --> at.in++;

        dnsServer.serverOut --> dnsQuerier.querierIn;
        dnsQuerier.querierOut --> dnsServer.serverIn;
}

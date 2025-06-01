package s2f.architecture.dns.resolver;

import s2f.architecture.dns.db.DnsDb;
import s2f.architecture.dns.client.DnsClientService;
import inet.node.inet.StandardHost;

//
// DNS server resolver model
// 
// In fact, this is a DNS recursive resolver implementation. It's composed of:
// - DnsResolverServer: The "brain" behind all the resolution logic. It also acts as the frontend for clients
// - DnsDb: The database that holds all of the records
// - DnsClientService: The DNS "client", as the resolver also needs to ask questions 
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-01-20
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

        client: DnsClientService {
            @display("p=749.656,56.45");
        }
    connections:
        resolver.socketIn <-- at.out++;
        resolver.socketOut --> at.in++;
		
		client.socketIn <-- at.out++;
        client.socketOut --> at.in++;
        
        resolver.serverIn <-- client.clientOut;
        resolver.serverOut --> client.clientIn;
}

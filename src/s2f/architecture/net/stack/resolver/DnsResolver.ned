package s2f.architecture.net.stack.resolver;
import s2f.architecture.dns.client.DnsClientService;
import s2f.architecture.dns.db.DnsDb;

// Module for resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
module DnsResolver
{
    parameters:
        string ispResolver;
        
    gates:
        input clientIn @directIn;
        input socketIn;
        output socketOut;
    submodules:
        stubResolver: StubDnsResolver {
            @display("p=103,57");
            ispResolver = parent.ispResolver;
        }
        dnsClient: DnsClientService {
            @display("p=103,154");
        }
        db: DnsDb {
            @display("p=178,115");
        }
    connections:
        clientIn --> stubResolver.clientIn;

        stubResolver.resolverOut --> dnsClient.clientIn;
        dnsClient.clientOut --> stubResolver.resolverIn;

        dnsClient.socketOut --> socketOut;
        dnsClient.socketIn <-- socketIn;
}

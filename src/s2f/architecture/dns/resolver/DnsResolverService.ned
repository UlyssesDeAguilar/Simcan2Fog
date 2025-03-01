package s2f.architecture.dns.resolver;

simple DnsResolverService
{
    parameters:
    	@class(dns::DnsResolverService);
    	string dbPath = default("^.db");
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input serverIn;
        output serverOut;
}

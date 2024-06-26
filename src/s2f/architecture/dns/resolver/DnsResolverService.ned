package s2f.architecture.dns.resolver;

simple DnsResolverService
{
    parameters:
    	@class(dns::DnsResolverService);
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input serverIn;
        output serverOut;
}

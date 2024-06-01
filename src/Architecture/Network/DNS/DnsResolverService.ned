package simcan2.Architecture.Network.DNS;

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

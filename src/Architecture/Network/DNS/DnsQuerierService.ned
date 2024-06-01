package simcan2.Architecture.Network.DNS;

simple DnsQuerierService
{
    parameters:
    	@class(dns::DnsQuerierService);
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input querierIn;
        output querierOut;
}
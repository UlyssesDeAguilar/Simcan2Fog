package s2f.architecture.dns.resolver;

//
// DNS resolver model
// 
// The implementation is inspired in the RFC 1034/1035
// yet it doesn't aim to be fully faithful
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-01-20
simple DnsResolverService
{
    parameters:
    	string dbPath = default("^.db"); // DNS database path
    gates:
        input socketIn @labels(ITransportPacket/up); // Socket input
        output socketOut @labels(ITransportPacket/down); // Socket output
        input serverIn; // Server input
        output serverOut; // Server output
}

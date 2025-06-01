package s2f.architecture.dns.client;

//
// Client that handles DNS querys with retransmission
// 
// Beware, it assumes that the sent packages via DnsClientCommand are
// valid DNS queries. No further processing is done by this module
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple DnsClientService
{
    parameters:
        int ticksForTimeOut = default(5); // Number of ticks before a timeout happens
        int timePerTick @unit(ms) = default(200 ms); // Time between ticks
        int maxAttempts = default(3); // Maximum number of attempts for each resolver server
        @display("i=block/rxtx,,0");
    gates:
        input socketIn @labels(ITransportPacket/up);    // Socket input
        output socketOut @labels(ITransportPacket/down); // Socket output
        input clientIn; // Client input
        output clientOut; // Client output
}

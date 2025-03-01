package s2f.architecture.dns.client;

simple DnsClientService
{
    parameters:
        @class(dns::DnsClientService);
        int ticksForTimeOut = default(5);
        int timePerTick @unit(ms) = default(200 ms);
        int maxAttempts = default(3);
        @display("i=block/rxtx,,0");
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input clientIn;
        output clientOut;
}

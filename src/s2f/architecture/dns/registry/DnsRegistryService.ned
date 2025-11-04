package s2f.architecture.dns.registry;
import inet.applications.contract.IApp;

//
// Module for CRUD operations on DNS records
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsRegistryService like IApp {
    parameters:
        string dbPath = default("^.db");    
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
    gates:
        input socketIn @labels(ITransportPacket/up); // Socket input
        output socketOut @labels(ITransportPacket/down); // Socket output
}

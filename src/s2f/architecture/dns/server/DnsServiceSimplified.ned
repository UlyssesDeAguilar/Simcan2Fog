package s2f.architecture.dns.server;
import inet.applications.contract.IApp;

// DNS server model
// 
// The implementation is inspired in the RFC 1034/1035
// yet it doesn't aim to be fully faithful
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsServiceSimplified like IApp {
    parameters:
        string dbPath = default("^.db"); // Path to the DNS database
        string authoritativeDomain = default(""); // The authoritative domain of this DNS server
    gates:
        input socketIn @labels(ITransportPacket/up); // Socket input
        output socketOut @labels(ITransportPacket/down); // Socket output
}
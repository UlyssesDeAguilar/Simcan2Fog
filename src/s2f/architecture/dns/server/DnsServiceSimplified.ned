package s2f.architecture.dns;
import inet.applications.contract.IApp;

// Module for registering and resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsServiceSimplified like IApp {
    parameters:
    	@class(dns::DnsServiceSimplified);
        string dbPath = default("^.db");
        string authoritativeDomain = default("");
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
package s2f.architecture.dns.registry;
import inet.applications.contract.IApp;

// Module for registering and resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsRegistryService like IApp {
    parameters:
    	@class(dns::DnsRegistryService);
        int mode = default(0);              // 0 for IDLE, 1 for NET_SCAN
        string dbPath = default("^.db");    
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
package simcan2.Architecture.Network.DNS;
import inet.applications.contract.IApp;

// Module for registering and resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsService like IApp {
    parameters:
    	@class(dns::DnsService);
    	string serverName;
        string mode;
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
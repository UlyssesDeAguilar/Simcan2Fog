package simcan2.Architecture.Network.DNS.Service;
import inet.applications.contract.IApp;

// Module for registering and resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DNS_Service like IApp {
    parameters:
    	@class(dns::DNS_Service);
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
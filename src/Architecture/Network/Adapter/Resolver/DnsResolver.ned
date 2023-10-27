package simcan2.Architecture.Network.Adapter.Resolver;
import inet.applications.contract.IApp;

// Module for resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsResolver like IApp {
    parameters:
    	@class(DnsResolver);
    gates:
        // Must be connected dynamically because of the definition 
        // of the IApp interface and the module composition of NetworkAdapter
        input moduleIn @loose;
        output moduleOut @loose;
		
		// These labels help the visualizer
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
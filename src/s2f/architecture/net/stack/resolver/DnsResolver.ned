package s2f.architecture.net.stack.resolver;
import inet.applications.contract.IApp;

// Module for resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple DnsResolver like IApp {
    parameters:
    	@class(DnsResolver);
        string ispResolver = default("7.7.7.7");
    gates:
		// These labels help the visualizer
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
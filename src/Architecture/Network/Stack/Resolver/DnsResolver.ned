package simcan2.Architecture.Network.Stack.Resolver;
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
        input moduleIn @loose;
        output moduleOut @loose;
		
		// These labels help the visualizer
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
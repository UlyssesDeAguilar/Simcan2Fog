package s2f.architecture.net.stack.proxy;
import inet.applications.contract.IApp;

// Module for resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple AppProxy like IApp {
    parameters:
    	@class(AppProxy);
        int listeningPort = default(443);
    gates:
		// These labels help the visualizer
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
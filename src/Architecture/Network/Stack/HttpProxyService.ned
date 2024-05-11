package simcan2.Architecture.Network.Stack;
import inet.applications.contract.IApp;

// HttpClientService
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-05-05
simple HttpProxyService like IApp {
    parameters:
    	@class(HttpProxyService);
        int port = 443;         // For HTTPS Protocol
    gates:        
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
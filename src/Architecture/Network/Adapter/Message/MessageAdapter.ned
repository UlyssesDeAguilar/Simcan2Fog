package simcan2.Architecture.Network.Adapter.Message;
import inet.applications.contract.IApp;

// Module for message translation between cSIMCAN_Message
// and Packet (from INET) coming from a TCP connection
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-09-10
simple MessageAdapter like IApp {
    parameters:
    	@class(MessageAdapter);
    gates:
        // Must be connected dynamically because of the definition 
        // of the IApp interface and the module composition of NetworkAdapter
        input moduleIn @loose;
        output moduleOut @loose;
		
		// These labels help the visualizer
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
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
        input moduleIn;
        output moduleOut;

        input socketIn;     // Is it inherited from the interface ?
        output socketOut;
}
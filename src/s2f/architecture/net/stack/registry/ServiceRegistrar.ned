package s2f.architecture.net.stack.registry;
import inet.applications.contract.IApp;

// Module for registering and unregistering services
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple ServiceRegistrar like IApp {
    parameters:
        string nameServerIp = default("");
        
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
}
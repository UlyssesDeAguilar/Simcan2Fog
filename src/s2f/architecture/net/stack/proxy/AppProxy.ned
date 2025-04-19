package s2f.architecture.net.stack.proxy;

// Module for proxying tcp/rest messages
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple AppProxy
{
    parameters:
        @class(AppProxy);
        int localPort = default(443);
        string serviceTablePath = default("^.serviceTable");
        @display("i=block/join,,0");
    gates:
        input transportIn @labels(ITransportPacket/up);
        output transportOut @labels(ITransportPacket/down);
        input appIn[];
        output appOut[];
}

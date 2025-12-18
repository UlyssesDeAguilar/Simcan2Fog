package s2f.apps.p2p;
import s2f.apps.UserAppModule;
import s2f.architecture.p2p.pow.PowP2P;
import s2f.apps.p2p.discovery.dns.DnsDiscoveryService;
import s2f.architecture.p2p.pow.nodes.FullNode;

module P2Protocol
{
    parameters:
        int pid = default(-1);
        int vmId = default(0);
        string resolverPath = default("");
        string p2pProto = default("s2f.architecture.p2p.pow.PowP2P");
        volatile bool defaultInitialized @mutable = default(true);
        @display("bgb=221,131");
    gates:
        input fromHub;
        output toHub;
        input socketIn;
        output socketOut;
        input serviceSocketIn[];
        output serviceSocketOut[];


    submodules:
        p2p: PowP2P {
            @display("p=110,53;i=app");
        }
        //discoveryService: DnsDiscoveryService;
        //node: FullNode {
        //    @display("p=181.57501,87.075005");
        //}
    connections allowunconnected:
        fromHub --> p2p.in;
        toHub <-- p2p.out;
        socketIn --> p2p.socketIn;
        socketOut <-- p2p.socketOut;
        //p2p.discoveryIn++ <-- discoveryService.out;
        //p2p.discoveryOut++ --> discoveryService.in;
        //serviceSocketIn++ --> discoveryService.socketIn;
        //serviceSocketOut++ <-- discoveryService.socketOut;
        //p2p.internalOut++ --> node.in;
        //p2p.internalIn++ <-- node.out;
        //node.out --> node.in;
}

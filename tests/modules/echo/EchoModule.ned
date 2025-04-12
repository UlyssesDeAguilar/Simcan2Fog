package echo;
import s2f.core.cSIMCAN_Core;
import s2f.architecture.net.card.NetworkCard;

module EchoModule {
    parameters:
        bool recvMode;
        int ip;
        int baseIp = default(0);
        int addressRange = default(0);
        
        mod.ip = this.ip;
        mod.baseIp = this.baseIp;
        mod.recvMode = this.recvMode;
        mod.addressRange = this.addressRange;
        netCard.L2Address = ip;
    gates:
        inout comm;
    submodules:
        netCard: NetworkCard;
        mod: Echo;
    connections:
        mod.networkOut --> netCard.cardIn;
        mod.networkIn <-- netCard.cardOut;
        netCard.comm <--> comm;
}
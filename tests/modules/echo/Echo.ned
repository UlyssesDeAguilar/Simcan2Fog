package echo;
import s2f.core.cSIMCAN_Core;

simple Echo extends cSIMCAN_Core{
    parameters:
        @class(Echo);
        bool recvMode;
        int ip;
        int baseIp = default(0);
        int addressRange = default(0);
        showInitValues = false;
    gates:
        inout comm;
}
package echo;

simple Echo 
{
    parameters:
        @class(Echo);
        bool recvMode;
        int ip;
        int baseIp = default(0);
        int addressRange = default(0);
    gates:
        input networkIn;
        output networkOut;
}
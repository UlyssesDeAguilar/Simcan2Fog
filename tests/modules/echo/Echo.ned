package s2f.tests.modules.echo;

simple Echo 
{
    parameters:
        bool recvMode;
        int ip;
        int baseIp = default(0);
        int addressRange = default(0);
    gates:
        input networkIn;
        output networkOut;
}
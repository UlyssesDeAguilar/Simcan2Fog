package ping;

simple PingClient
{
    parameters:
        @class(PingStackDriver);
        bool server = false;
        int numPings = 4;
    gates:
        inout comm;
}
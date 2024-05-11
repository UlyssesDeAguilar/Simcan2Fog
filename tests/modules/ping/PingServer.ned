package ping;

simple PingServer
{
    parameters:
        @class(PingStackDriver);
        bool server = true;
    gates:
        inout comm;
}
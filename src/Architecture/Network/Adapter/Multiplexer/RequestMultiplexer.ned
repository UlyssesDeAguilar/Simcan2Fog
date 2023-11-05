package simcan2.Architecture.Network.Adapter.Multiplexer;
//import simcan2.Core.cSIMCAN_Core;

simple RequestMultiplexer //extends cSIMCAN_Core
{
    parameters:
        @class(RequestMultiplexer);
    gates:
        // General I/O
        input gInput;
        output gOutput;

        // Multiplexed I/O
        output ResolverIn @loose;
        input  ResolverOut @loose;

        output MessageIn @loose;
        input  MessageOut @loose;
}
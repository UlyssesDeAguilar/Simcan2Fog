package s2f.architecture.net.stack.resolver;

// Module for resolving addresses
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-10-6
simple StubDnsResolver
{
    parameters:
        @class(StubDnsResolver);
        string ispResolver;
        string dbPath = default("^.db");
        @display("i=block/app,,0");
    gates:
		// These labels help the visualizer
        input clientIn;
        input resolverIn;
        output resolverOut;
}

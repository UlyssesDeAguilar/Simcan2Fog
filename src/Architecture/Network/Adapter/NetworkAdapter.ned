package simcan2.Architecture.Network.Adapter;

import simcan2.Architecture.Network.Adapter.Message.MessageAdapter;
import simcan2.Architecture.Network.Adapter.Resolver.DNS_Resolver;
import simcan2.Architecture.Network.Adapter.Multiplexer.RequestMultiplexer;
import inet.node.inet.StandardHost;

// Module for communicating between INET framework and SIMCAN modules.
// Ideally everything would work under said framework but this class provides
// a way to interconnect the modules without changing anything
// 
// The design goal is to achieve the following communication:
//            Simcan <--> INET network <--> Simcan
//            INET (network + module) <--> Simcan (Must be always Simcan requests/responses)
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2023-09-10
module NetworkAdapter extends StandardHost
{
parameters:
    @display("i=device/card");     // We have to make an icon (default is a PC)
    numApps = 2;      // Static assignment

    // Instantiate the core applications
    app[0].typename = "MessageAdapter";
    app[1].typename = "DNS_Resolver";
gates:
    input moduleIn;
    output moduleOut;
submodules:
    rm: RequestMultiplexer;
connections allowunconnected:
    moduleIn --> rm.gInput;
    moduleOut <-- rm.gOutput;
}
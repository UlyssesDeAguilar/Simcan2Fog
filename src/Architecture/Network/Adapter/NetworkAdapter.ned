package simcan2.Architecture.Network.Adapter;

import simcan2.Architecture.Network.Adapter.Message.*;
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
    @display("");     // We have to make an icon (default is a PC)
    numApps = 1;      // Static assignment

    // Instantiate the core applications
    app[0].typename = "MessageAdapter";
gates:
    input moduleIn;
    output moduleOut;
}
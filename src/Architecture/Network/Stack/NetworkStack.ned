package simcan2.Architecture.Network.Stack;

import inet.node.inet.StandardHost;
import simcan2.Architecture.Network.Stack.StackMultiplexer;


// This is the main network technology stack for hypervisors.
//
// It contains (for now) an HttpClient, HttpProxy and the DNS_Resolver
//
// They work closely with the hypervisor in order to provide and consume services
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-05-05
//
module NetworkStack extends StandardHost
{
parameters:
    @display("i=device/card");     // We have to make an icon (default is a PC)
    numApps = 4;
    app[0].typename = "HttpClientService";
    app[1].typename = "HttpProxyService";
    app[2].typename = "DNS_Resolver";
    app[3].typename = "DNS_Service";
gates:
    inout eth_comm;

submodules:
    sm: StackMultiplexer;

connections:
    eth_comm <--> sm.comm;
}
package s2f.tests.networks;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import s2f.Architecture.Network.DNS.DNS_Server;
import s2f.Architecture.Network.Adapter.NetworkAdapter;
import endpoint.Endpoint;
import inet.node.inet.Router;
import inet.node.ethernet.EtherSwitch;
import inet.node.ethernet.Eth10M;

module TestStub 
{
    gates:
        inout comm;
    submodules:
        networkAdapter: NetworkAdapter;
    connections allowunconnected:
        comm <--> networkAdapter.ethg++;
}

module EndpointModule extends TestStub
{
    submodules:
        end: Endpoint;
    connections:
        networkAdapter.moduleIn  <-- end.endpointOut;
        networkAdapter.moduleOut --> end.endpointIn;
}

network DNSNet2
{
    submodules:
        configurator: Ipv4NetworkConfigurator;
        endpoint: EndpointModule;
        dc_DataCentre: TestStub;
        dns_service: DNS_Server
        {
            isMain = true;
            debug = true;
        }

        bc_router: Router;
        switch: EtherSwitch;
    connections:
        switch.ethg++ <--> Eth10M <--> endpoint.comm;
        switch.ethg++ <--> Eth10M <--> dc_DataCentre.comm;
        bc_router.ethg++ <--> Eth10M <--> dns_service.ethg++;
        switch.ethg++ <--> Eth10M <--> bc_router.ethg++;
}
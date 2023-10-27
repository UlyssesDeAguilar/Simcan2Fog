package simcan2.Architecture.Network.DNS;
import inet.node.inet.StandardHost;
import simcan2.Architecture.Network.DNS.Service.DNS_Service;

module DNS_Server extends StandardHost
{
parameters:
    @display("");                
    bool isMain = default(false);   
    
    // Prepare and instantiate the service
    numApps = 1;
    app[0].typename = "DNS_Service";
}

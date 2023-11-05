package simcan2.Architecture.Network.DNS;
import inet.node.inet.StandardHost;
import simcan2.Architecture.Network.DNS.Service.DNS_Service;

module DNS_Server extends StandardHost
{
parameters:
    @display("i=dns");
    string ipDump = default("ips.xml"); // Name of the ips dump file            
    bool isMain = default(false);       // Flag for determining if it is a TLD DNS (maybe fix name?)
    bool debug = default(false);        // Flag for printing debug information (topology detected, current registries, etc ...)
    
    // Prepare and instantiate the service
    numApps = 1;
    app[0].typename = "DNS_Service";
}

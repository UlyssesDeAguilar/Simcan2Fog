import s2f.Core.SimData.SimData;
import s2f.Architecture.Nodes.Iot.*;
import s2f.Architecture.Nodes.Fog.FogNode;
import s2f.Architecture.Network.Adapter.NetworkAdapter;
import DcDriver.SimpleDc;
import DcDriver.DcDriver;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

network SimpleDcNetwork
{
    submodules:

        simData : SimData;
        
        dc_DataCentre: SimpleDc {	
            	@display("p=717,62");
        }

        driver: DcDriver {
            @display("p=363,119");
        }

        netAdapter1: NetworkAdapter {
            @display("p=450,119");
        }

        router_bb: Router{
            @display("i=abstract/router2");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=48,35");
        }
    
    connections:
        // Simplified network composition
        netAdapter1.ethg++ <--> Eth10M <--> router_bb.ethg++;
        dc_DataCentre.comm++ <--> Eth10M <--> router_bb.ethg++;

        // Pipeline for network communications
        driver.out --> netAdapter1.moduleIn;
        driver.in <-- netAdapter1.moduleOut;
}

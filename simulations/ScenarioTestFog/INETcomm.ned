//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package s2f.simulations.ScenarioTestFog;

import s2f.Core.SimData.SimData;
import s2f.Architecture.Nodes.Iot.*;
import s2f.Architecture.Nodes.Fog.FogNode;
import s2f.Architecture.Network.Adapter.NetworkAdapter;
import s2f.Architecture.Network.DNS.DNS_Server;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;

network INETComm
{
    parameters:
        string userList;
        string dataCentresList;
        int numControllers = default(2);
        int numSensors = default(2);

    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centres
        // ---------------------------------------------------------------
        simData : SimData;
        
        dc_DataCentre[1]: DataCentre_A {	
            	@display("p=717,62");
        }

        fog_node: FogNode {
            @display("p=363,119");
        }

        netAdapter1: NetworkAdapter {
            @display("p=450,119");
        }

        router0: Router {
            @display("p=521,186");
        }
        router1: Router {
            @display("p=607,228");
        }
        router2: Router {
            @display("p=607,144");
        }

        router_bb: Router{
            @display("i=abstract/router2");
        }
        dns_server: DNS_Server {
            isMain = true;
            debug = true;
            @display("p=556,97");
        }

        controller[numControllers]: IotController;
        sensor[numSensors*numControllers]: IotSensor;
        configurator: Ipv4NetworkConfigurator {
            @display("p=48,35");
        }
    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between the modules and their network adapters	
        fog_node.toDataCentre --> netAdapter1.moduleIn;
        fog_node.fromDataCentre <-- netAdapter1.moduleOut;

        // dc_DataCentre[0].toCloudProvider --> netAdapter2.moduleIn;
        // dc_DataCentre[0].fromCloudProvider <-- netAdapter2.moduleOut;

		// Simpler connections (will be parametrized later with NetworkAdapter)
        for i=0..numControllers-1 {
            fog_node.toEdge++ --> controller[i].fromFog;
            fog_node.fromEdge++ <-- controller[i].toFog;
        }

        for i=0..numControllers-1, for j=0..numSensors-1 {
            controller[i].toIot++ --> sensor[j+i*numSensors].in;
            controller[i].fromIot++ <-- sensor[j+i*numSensors].out;
        }

        // "Real" network composition
        netAdapter1.ethg++ <--> Eth10M <--> router0.ethg++;
        dc_DataCentre[0].comm++ <--> Eth10M <--> router2.ethg++;

        // Triangular router config
        router0.ethg++ <--> Eth10M <--> router_bb.ethg++;
        router1.ethg++ <--> Eth10M <--> router_bb.ethg++;
        router2.ethg++ <--> Eth10M <--> router_bb.ethg++;
        router_bb.ethg++ <--> Eth10M <--> dns_server.ethg++;

}

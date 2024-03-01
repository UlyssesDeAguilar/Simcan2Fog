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
package DcDriver;

import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.Adapter.NetworkAdapter;
import simcan2.Management.DataCentreManagers.**;


module SimpleDc
{
    parameters:
        @smlevel(0);
        int numBlades;
        manager.minActiveNodes = numBlades;
        @display("i=data-center;bgb=756,453");
    gates:
        inout comm[];

    submodules:
        dcManager: DataCentreManagerBase;
        resourceManager: DcResourceManager;

        switch: Switch;
        networkAdapter: NetworkAdapter;
        
        blade[numBlades] : CloudBlade {
            address = index;
        }
        
    connections allowunconnected:

		// Connect the ethernet wiring to the networkAdapter
        for i=0..sizeof(comm)-1 {
            networkAdapter.ethg++ <--> comm[i];
        }
        
        // Connections between Data-Centre manager and Data-centre outside
        dcManager.networkOut --> ned.IdealChannel --> networkAdapter.moduleIn;
        dcManager.networkIn <-- ned.IdealChannel <-- networkAdapter.moduleOut;
		
		// Connect the main switch with the Manager
        switch.upper <-->  dcManager.localNetwork;
		
        for i=0..numBlades-1 {
            switch.comm++ <--> blade[i].comm;
        }
}

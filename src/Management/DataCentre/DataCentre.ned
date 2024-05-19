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
package simcan2.Management.DataCentre;

import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.Stack.NetworkStack;
import simcan2.Management.DataCentreManagers.DataCentreManagerBase.DataCentreManagerBase;
import simcan2.Management.DataCentreManagers.ResourceManager.DcResourceManager;

module DataCentre
{
    parameters:
        @smlevel(0);
        int numBlades;
        dcManager.minActiveMachines = numBlades;	// Test the capabilities afterwards!
        @display("i=data-center;bgb=571,307");
    gates:
        inout comm[];

    submodules:
        dcManager: DataCentreManagerBase {
            @display("p=181,94");
        }
        resourceManager: DcResourceManager {
            @display("p=75,47");
        }
        switch: Switch {
            @display("p=288,138");
        }
        stack: NetworkStack {
			hasCloudEvents = true;
            @display("p=71,137");
        }
        // Blades of the Datacentre
        blade[numBlades]: CloudBlade {
            address = index;
            @display("p=430,138");
        }

    connections allowunconnected:

		// Connect the ethernet wiring to the networkAdapter
        for i=0..sizeof(comm)-1 {
            stack.ethg++ <--> comm[i];
        }

        // Connections between Data-Centre manager and Data-centre outside
        dcManager.networkOut --> stack.queueIn;
        dcManager.networkIn <-- stack.queueOut;
		dcManager.eventOut --> stack.eventsIn;

		// Connect the main switch with the Manager
        switch.manager <--> dcManager.localNetwork;
        switch.netStack <--> stack.eth_comm;

        for i=0..numBlades-1 {
            switch.comm++ <--> blade[i].comm;
        }
}


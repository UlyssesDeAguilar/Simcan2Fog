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

package s2f.architecture.nodes.fog;
import s2f.architecture.net.stack.RoutingNetworkStack;
import s2f.architecture.nodes.cloud.CloudBlade;
import s2f.architecture.net.switch.Switch;
import s2f.management.managers.DcManager;
import s2f.management.managers.ResourceManager;

module FogNode 
{
	parameters:
        @smlevel(0);
        @servicenode();
        
        int numBlades;
        string serviceDeployed;
        dcManager.minActiveMachines = numBlades;	// Test the capabilities afterwards!
        **.parentPath = "";
        **.nsPath = "";
        @display("i=abstract/router;bgb=571,307");
    gates:
        inout comm[];

    submodules:
        dcManager: DcManager {
            @display("p=181,94");
        }
        resourceManager: ResourceManager {
            @display("p=75,47");
        }
        switch: Switch {
            @display("p=288,138");
        }
        
        stack: RoutingNetworkStack {
            hasTcp=true;
            hasUdp=true;
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
        //switch.netStack <--> stack.eth_comm;

        for i=0..numBlades-1 {
            switch.comm++ <--> blade[i].comm;
        }
        
        for i=0..numBlades-1, for j=0..(blade[i].maxUsers * blade[i].numApps)-1 {
            blade[i].socketIn++ <-- {@display("ls=,0");} <-- stack.ssOut++;
            blade[i].socketOut++ --> {@display("ls=,0");} --> stack.ssIn++;
        }
}

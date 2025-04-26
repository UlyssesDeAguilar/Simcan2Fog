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
import s2f.architecture.nodes.cloud.CloudBlade;
import s2f.architecture.net.stack.RoutingNetworkStack;
import s2f.architecture.net.switch.Switch;
import s2f.architecture.net.db.SimpleNetDb;
import s2f.architecture.net.card.NetworkCard;
import s2f.architecture.net.stack.proxy.AppProxy;
import s2f.architecture.net.stack.proxy.ServiceTable;
import s2f.management.managers.DcManager;
import s2f.management.managers.ResourceManager;
import s2f.management.managers.NodePool;

module FogNode 
{
	parameters:
        @smlevel(0);
        @servicenode();
        
        int numBlades;
        //string serviceDeployed;
        string zone;
        string domain = default(zone + ".com");
        dcManager.minActiveMachines = numBlades;
		dcManager.nodeName = fullName();
		dcManager.zone = this.zone;
		**.netDbPath = fullPath() + ".netDb";
		**.resolverPath = fullPath() + ".stack.dnsResolver";
		**.resourceManagerPath = fullPath() + ".resourceManager";
        resourceManager.nodePoolPath = "^.pool";
        resourceManager.selectionStrategy = default("dc::FirstFit");
        stack.nodeTopic = fullName();
        pool.estimatedNumberOfNodes = this.numBlades;
        serviceTable.domain = this.domain;
        
        @display("i=abstract/router;bgb=571,307");
    gates:
        inout comm[];

    submodules:
        netCard: NetworkCard {

            @display("p=234,120");
        }
        resourceManager: ResourceManager {
            @display("p=64,33");
        }
        pool: NodePool {
            @display("p=163,33");
        }
        netDb: SimpleNetDb {
            @display("p=242,33");
        }
        switch: Switch {
            @display("p=297,187");
        }
        
        stack: RoutingNetworkStack {
            hasTcp=true;
            hasUdp=true;
            @display("p=71,137");
        }
        
        // Blades of the Datacentre
        blade[numBlades]: CloudBlade {
            address = index;
            @display("p=422,187");
        }

        proxy: AppProxy {
            @display("p=154,187");
        }
        serviceTable: ServiceTable {

            @display("p=325,33");
        }
		dcManager: DcManager {
            @display("p=154,120");
        }

    connections:

		// Connect the ethernet wiring to the networkAdapter
        for i=0..sizeof(comm)-1 {
            stack.ethg++ <--> comm[i];
        }

        // Connections between Data-Centre manager and Data-centre outside
        dcManager.queueOut --> stack.queueIn;
        dcManager.queueIn <-- stack.queueOut;

        // Connect the main switch with the Manager
        dcManager.networkIn <-- netCard.cardOut;
        dcManager.networkOut --> netCard.cardIn;
        switch.comm++ <--> netCard.comm;

        for i=0..numBlades-1 {
            switch.comm++ <--> blade[i].comm;
        }

        for i=0..numBlades-1, for j=0..(blade[i].maxUsers * blade[i].numApps)-1 {
            blade[i].socketIn++ <-- { @display("ls=,0"); } <-- proxy.appOut++;
            blade[i].socketOut++ --> { @display("ls=,0"); } --> proxy.appIn++;
        }

        proxy.transportIn <-- stack.ssOut++;
        proxy.transportOut --> stack.ssIn++;
}

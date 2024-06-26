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

import s2f.Architecture.Nodes.Cloud.CloudBlade;
import s2f.Architecture.Nodes.Cloud.CloudBoard;
import s2f.Architecture.Nodes.Cloud.CloudRack;
import s2f.Architecture.Network.Switch.Switch;
import s2f.Architecture.Network.Adapter.NetworkAdapter;
import s2f.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;
import s2f.Management.CloudProviders.*.*;
import s2f.Architecture.Network.DataCentreNetwork.*;

// ---------------------------------------------------------------
//   Definition of the data-centre DataCentre_A
// ---------------------------------------------------------------
module DataCentre_A
{
    parameters:
        @smlevel(0);
        int numBlades;

        // Configure the main switch
        // switch.level = 0;

        @display("i=data-center;bgb=756,453");
    gates:
        inout comm[];

    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centre manager
        // ---------------------------------------------------------------
        dcManager: DataCentreManagerFirstFit {
            parameters:
                @display("p=197,207");
        }

        networkAdapter: NetworkAdapter {
            @display("p=83,207");
        }
        
        switch: Switch {
            @display("p=441,206");
        }

        blade[numBlades] : CloudBlade {
            address = index;
        }
        
    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
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
		
        // TODO: In the future build dynamically the Racks/Boards
        // Connect the switch with the blades
        for i=0..numBlades-1 {
            switch.comm++ <--> blade[i].comm;
        }
}

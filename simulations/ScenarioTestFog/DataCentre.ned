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

package simcan2.simulations.ScenarioTestFog;

import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Nodes.Cloud.CloudBoard;
import simcan2.Architecture.Nodes.Cloud.CloudRack;
import simcan2.Architecture.Network.Adapter.NetworkAdapter;
import simcan2.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;
import simcan2.Management.CloudProviders.*.*;
import simcan2.Architecture.Network.DataCentreNetwork.*;

// ---------------------------------------------------------------
//   Definition of the data-centre DataCentre_A
// ---------------------------------------------------------------
module DataCentre_A
{
    parameters:
        string appList;
        string vmList;
        string slaList;
        string userList;
        int numCmpRacksA;
        int numCmpRacksB;
        int numStoRacks = 0;

        @display("i=data-center");
    gates:
        inout comm[];
    
    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centre manager
        // ---------------------------------------------------------------
        dcManager: DataCentreManagerFirstFit {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                @display("p=193,148");
        }

        networkAdapter: NetworkAdapter;

        rackCmp_Rack_A[numCmpRacksA]: CloudRack {
            @display("p=154,51");
        }

        rackCmp_Rack_B[numCmpRacksB]: CloudRack {
            @display("p=502,99");
        }

        rackSto_Rack_B[numStoRacks]: CloudRack {
            @display("p=400,30");
        }


        // ---------------------------------------------------------------
        //   Definition of Network
        // ---------------------------------------------------------------
        dataCentreNetwork: DataCentreNetwork {
            @display("p=325,126");
            gates:
                inComm[numCmpRacksA+numCmpRacksB];
                outComm[numCmpRacksA+numCmpRacksB];
                inStorage[numStoRacks];
                outStorage[numStoRacks];
        }

    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between Data-Centre manager and Data-centre outside
        dcManager.out --> ned.IdealChannel --> networkAdapter.moduleIn;
        dcManager.in <-- ned.IdealChannel <-- networkAdapter.moduleOut;

        // Jump the ethernet wiring to the networkAdapter
        // WARING: Do not put any delay inbetween
        for i=0..sizeof(comm)-1{
            networkAdapter.ethg++ <--> comm[i];
        }

        // Connections between computing racks and the network
        for i=0..numCmpRacksA-1 {
            rackCmp_Rack_A[i].out --> DataCentreEth40G_channel --> dataCentreNetwork.inComm++;
            rackCmp_Rack_A[i].in <-- DataCentreEth40G_channel <-- dataCentreNetwork.outComm++;
        }

        for i=0..numCmpRacksB-1 {
            rackCmp_Rack_B[i].out --> DataCentreEth40G_channel --> dataCentreNetwork.inComm++;
            rackCmp_Rack_B[i].in <-- DataCentreEth40G_channel <-- dataCentreNetwork.outComm++;
        }

        // Connections between storage racks and the network
        for i=0..numStoRacks-1 {
            rackSto_Rack_B[i].out --> DataCentreEth40G_channel --> dataCentreNetwork.inStorage++;
            rackSto_Rack_B[i].in <-- DataCentreEth40G_channel <-- dataCentreNetwork.outStorage++;
        }
}
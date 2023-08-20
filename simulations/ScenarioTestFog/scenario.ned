package simcan2.simulations.ScenarioTestFog;

import simcan2.Architecture.Nodes.Fog.FogNode;
import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Nodes.Cloud.CloudBoard;
import simcan2.Architecture.Nodes.Cloud.CloudRack;
import simcan2.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;
import simcan2.Management.CloudProviders.*.*;
import simcan2.Architecture.Network.DataCentreNetwork.*;
import simcan2.Management.UserGenerators.*.*;

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

        @display("bgb=631,231");
    gates:
        input fromCloudProvider;
        output toCloudProvider;

    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
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

        // ---------------------------------------------------------------
        //   Definition of Computing Racks
        // ---------------------------------------------------------------
        rackCmp_Rack_A[numCmpRacksA]: CloudRack {
            @display("p=154,51");
        }

        rackCmp_Rack_B[numCmpRacksB]: CloudRack {
            @display("p=502,99");
        }

                // ---------------------------------------------------------------
        //   Definition of Computing Racks
        // ---------------------------------------------------------------
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
        dcManager.out --> ned.IdealChannel --> toCloudProvider;
        dcManager.in <-- ned.IdealChannel <-- fromCloudProvider;

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

// ---------------------------------------------------------------
//   Definition of the scenario
// ---------------------------------------------------------------
network ScenarioTest
{
    parameters:
        string appList;
        string vmList;
        string slaList;
        string userList;
        string dataCentresList;

        @display("bgb=610,278");
    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centres
        // ---------------------------------------------------------------
        dc_DataCentre[1]: DataCentre_A {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                @display("p=288,58");
        }

        fog_node: FogNode {
            @display("p=171,169");
        }
    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between the cloud provider and each data-centre manager
        fog_node.out --> ned.IdealChannel --> dc_DataCentre[0].fromCloudProvider;
        fog_node.in <-- ned.IdealChannel <-- dc_DataCentre[0].toCloudProvider;
}

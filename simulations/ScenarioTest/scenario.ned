package simcan2.simulations.ScenarioTest;

import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Nodes.Cloud.CloudBoard;
import simcan2.Architecture.Nodes.Cloud.CloudRack;
import simcan2.Management.DataCenterManager.DataCenterManager;
import simcan2.Management.CloudProviders.*.*;
import simcan2.Architecture.Network.DataCenterNetwork.*;
import simcan2.Management.UserGenerators.*.*;

// ---------------------------------------------------------------
//   Definition of the data-center DataCenter_A
// ---------------------------------------------------------------
module DataCenter_A
{
    parameters:
        string appList;
        string vmList;
        string slaList;
        string userList;

        @display("bgb=631,231");
    gates:
        input fromCloudProvider;
        output toCloudProvider;

    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-center manager
        // ---------------------------------------------------------------
        dcManager: DataCenterManager {
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
        rackCmp_Rack_A_0: CloudRack {

            @display("p=109,34");
        }
        // ---------------------------------------------------------------
        //   Definition of Network
        // ---------------------------------------------------------------
        dataCenterNetwork: DataCenterNetwork {
            @display("p=299,51");
            gates:
                inComm[1];
                outComm[1];
                inStorage[0];
                outStorage[0];
        }

    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between Data-Center manager and Data-center outside
        dcManager.out --> ned.IdealChannel --> toCloudProvider;
        dcManager.in <-- ned.IdealChannel <-- fromCloudProvider;

        // Connections between computing racks and the network
        rackCmp_Rack_A_0.out --> DataCenterEth40G_channel --> dataCenterNetwork.inComm++;
        rackCmp_Rack_A_0.in <-- DataCenterEth40G_channel <-- dataCenterNetwork.outComm++;

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
        string dataCentersList;

        @display("bgb=610,278");
    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centers
        // ---------------------------------------------------------------
        dc_DataCenter_A_0: DataCenter_A {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                @display("p=379,131");
        }

        // -------------------------------------------------------
        //   Definition of the cloud provider
        // -------------------------------------------------------
        cloudProvider: CloudProviderBase_firstBestFit {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                dataCentersList = dataCentersList;

                @display("p=240,76");
            gates:
                fromDataCenter[1];
                toDataCenter[1];
        }

        // -------------------------------------------------------
        //   Definition of the user generator
        // -------------------------------------------------------
        userGenerator: UserGenerator_simple {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                @display("p=60,125");
        }

    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between the cloud provider and each data-center manager
        cloudProvider.toDataCenter++ --> ned.IdealChannel --> dc_DataCenter_A_0.fromCloudProvider;
        cloudProvider.fromDataCenter++ <-- ned.IdealChannel <-- dc_DataCenter_A_0.toCloudProvider;

        cloudProvider.toUserGenerator --> ned.IdealChannel --> userGenerator.fromCloudProvider;
        userGenerator.toCloudProvider --> ned.IdealChannel --> cloudProvider.fromUserGenerator;

}

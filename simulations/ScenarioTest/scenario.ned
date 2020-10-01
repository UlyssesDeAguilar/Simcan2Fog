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
module DataCenter_A{

	parameters:
		string appList;
		string vmList;
		string slaList;
		string userList;

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
		dcManager:DataCenterManager{
			parameters:
				appList=appList;
				vmList=vmList;
				slaList=slaList;
				userList=userList;
		}

		// ---------------------------------------------------------------
		//   Definition of Computing Racks
		// ---------------------------------------------------------------
		rackCmp_Rack_A_0:CloudRack;
		rackSto_Rack_B_0:CloudRack;

		// ---------------------------------------------------------------
		//   Definition of Network
		// ---------------------------------------------------------------
		dataCenterNetwork:DataCenterNetwork{
			gates:
				inComm[1];
				outComm[1];
				inStorage[1];
				outStorage[1];
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
		rackSto_Rack_B_0.out --> DataCenterEth40G_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_0.in <-- DataCenterEth40G_channel <-- dataCenterNetwork.outStorage++;

}

// ---------------------------------------------------------------
//   Definition of the scenario
// ---------------------------------------------------------------
network ScenarioTest{

	parameters:
		string appList;
		string vmList;
		string slaList;
		string userList;
		string dataCentersList;

	// -------------------------------------------------------
	//   Definition of main modules
	// -------------------------------------------------------
	submodules:

		// ---------------------------------------------------------------
		//   Definition of the data-centers
		// ---------------------------------------------------------------
		dc_DataCenter_A_000:DataCenter_A{
			parameters:
				appList=appList;
				vmList=vmList;
				slaList=slaList;
				userList=userList;
		}

		// -------------------------------------------------------
		//   Definition of the cloud provider
		// -------------------------------------------------------
		cloudProvider:CloudProviderBase_firstBestFit{
			parameters:
				appList=appList;
				vmList=vmList;
				slaList=slaList;
				userList=userList;
				dataCentersList=dataCentersList;

			gates:
				fromDataCenter[1];
				toDataCenter[1];
		}

		// -------------------------------------------------------
		//   Definition of the user generator
		// -------------------------------------------------------
		userGenerator:UserGenerator_simple{
			parameters:
				appList=appList;
				vmList=vmList;
				slaList=slaList;
				userList=userList;
		}

	// ---------------------------------------------------------------
	//   Connections section
	// ---------------------------------------------------------------
	connections allowunconnected:

		// Connections between the cloud provider and each data-center manager
		cloudProvider.toDataCenter++ --> ned.IdealChannel --> dc_DataCenter_A_000.fromCloudProvider;
		cloudProvider.fromDataCenter++ <-- ned.IdealChannel <-- dc_DataCenter_A_000.toCloudProvider;

		cloudProvider.toUserGenerator --> ned.IdealChannel --> userGenerator.fromCloudProvider;
		userGenerator.toCloudProvider --> ned.IdealChannel --> cloudProvider.fromUserGenerator;

}

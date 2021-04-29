package simcan2.simulations.ScenarioTestNewGUI; 

import simcan2.Architecture.Nodes.Cloud.CloudBlade;
import simcan2.Architecture.Nodes.Cloud.CloudBoard;
import simcan2.Architecture.Nodes.Cloud.CloudRack;
import simcan2.Management.DataCenterManagers.*.*;
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
		string userList;
		string slaList;

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
				userList=userList;
				slaList=slaList;
		}

		// ---------------------------------------------------------------
		//   Definition of Computing Racks
		// ---------------------------------------------------------------
		rackCmp_Rack_A_000:CloudRack;
		rackCmp_Rack_A_001:CloudRack;
		rackCmp_Rack_A_002:CloudRack;
		rackCmp_Rack_A_003:CloudRack;
		rackCmp_Rack_A_004:CloudRack;
		rackCmp_Rack_A_005:CloudRack;
		rackCmp_Rack_A_006:CloudRack;
		rackCmp_Rack_A_007:CloudRack;
		rackCmp_Rack_A_008:CloudRack;
		rackCmp_Rack_A_009:CloudRack;

		// ---------------------------------------------------------------
		//   Definition of Storage Racks
		// ---------------------------------------------------------------
		rackSto_Rack_B_000:CloudRack;
		rackSto_Rack_B_001:CloudRack;
		rackSto_Rack_B_002:CloudRack;
		rackSto_Rack_B_003:CloudRack;
		rackSto_Rack_B_004:CloudRack;
		rackSto_Rack_B_005:CloudRack;
		rackSto_Rack_B_006:CloudRack;
		rackSto_Rack_B_007:CloudRack;
		rackSto_Rack_B_008:CloudRack;
		rackSto_Rack_B_009:CloudRack;
		rackSto_Rack_B_010:CloudRack;
		rackSto_Rack_B_011:CloudRack;
		rackSto_Rack_B_012:CloudRack;
		rackSto_Rack_B_013:CloudRack;
		rackSto_Rack_B_014:CloudRack;
		rackSto_Rack_B_015:CloudRack;
		rackSto_Rack_B_016:CloudRack;
		rackSto_Rack_B_017:CloudRack;
		rackSto_Rack_B_018:CloudRack;
		rackSto_Rack_B_019:CloudRack;
		rackSto_Rack_B_020:CloudRack;
		rackSto_Rack_B_021:CloudRack;
		rackSto_Rack_B_022:CloudRack;
		rackSto_Rack_B_023:CloudRack;
		rackSto_Rack_B_024:CloudRack;
		rackSto_Rack_B_025:CloudRack;
		rackSto_Rack_B_026:CloudRack;
		rackSto_Rack_B_027:CloudRack;
		rackSto_Rack_B_028:CloudRack;
		rackSto_Rack_B_029:CloudRack;
		rackSto_Rack_B_030:CloudRack;
		rackSto_Rack_B_031:CloudRack;
		rackSto_Rack_B_032:CloudRack;
		rackSto_Rack_B_033:CloudRack;
		rackSto_Rack_B_034:CloudRack;
		rackSto_Rack_B_035:CloudRack;
		rackSto_Rack_B_036:CloudRack;
		rackSto_Rack_B_037:CloudRack;
		rackSto_Rack_B_038:CloudRack;
		rackSto_Rack_B_039:CloudRack;
		rackSto_Rack_B_040:CloudRack;
		rackSto_Rack_B_041:CloudRack;
		rackSto_Rack_B_042:CloudRack;
		rackSto_Rack_B_043:CloudRack;
		rackSto_Rack_B_044:CloudRack;
		rackSto_Rack_B_045:CloudRack;
		rackSto_Rack_B_046:CloudRack;
		rackSto_Rack_B_047:CloudRack;
		rackSto_Rack_B_048:CloudRack;
		rackSto_Rack_B_049:CloudRack;

		// ---------------------------------------------------------------
		//   Definition of Network
		// ---------------------------------------------------------------
		dataCenterNetwork:DataCenterNetwork{
			gates:
				inComm[10];
				outComm[10];
				inStorage[50];
				inStorage[50];
		}

	// ---------------------------------------------------------------
	//   Connections section
	// ---------------------------------------------------------------
	connections allowunconnected:

		// Connections between Data-Center manager and Data-center outside
		dcManager.out --> ned.IdealChannel --> toCloudProvider;
		dcManager.in <-- ned.IdealChannel <-- fromCloudProvider;

		// Connections between computing racks and the network
		rackCmp_Rack_A_000.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_000.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_001.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_001.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_002.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_002.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_003.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_003.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_004.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_004.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_005.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_005.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_006.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_006.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_007.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_007.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_008.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_008.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;
		rackCmp_Rack_A_009.out --> DataCenterEth10M_channel --> dataCenterNetwork.inComm++;
		rackCmp_Rack_A_009.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outComm++;

		// Connections between storage racks and the network
		rackSto_Rack_B_000.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_000.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_001.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_001.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_002.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_002.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_003.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_003.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_004.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_004.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_005.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_005.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_006.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_006.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_007.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_007.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_008.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_008.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_009.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_009.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_010.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_010.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_011.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_011.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_012.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_012.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_013.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_013.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_014.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_014.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_015.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_015.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_016.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_016.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_017.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_017.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_018.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_018.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_019.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_019.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_020.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_020.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_021.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_021.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_022.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_022.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_023.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_023.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_024.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_024.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_025.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_025.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_026.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_026.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_027.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_027.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_028.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_028.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_029.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_029.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_030.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_030.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_031.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_031.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_032.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_032.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_033.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_033.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_034.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_034.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_035.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_035.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_036.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_036.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_037.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_037.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_038.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_038.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_039.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_039.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_040.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_040.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_041.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_041.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_042.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_042.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_043.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_043.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_044.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_044.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_045.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_045.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_046.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_046.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_047.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_047.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_048.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_048.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;
		rackSto_Rack_B_049.out --> DataCenterEth10M_channel --> dataCenterNetwork.inStorage++;
		rackSto_Rack_B_049.in <-- DataCenterEth10M_channel <-- dataCenterNetwork.outStorage++;

}

// ---------------------------------------------------------------
//   Definition of the scenario
// ---------------------------------------------------------------
network ScenarioTestNewGUI{

	parameters:
		string appList;
		string vmList;
		string userList;
		string dataCentersList;
		string slaList;

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
				userList=userList;
				slaList=slaList;
		}

		// -------------------------------------------------------
		//   Definition of the cloud provider
		// -------------------------------------------------------
		cloudProvider:CloudProviderBase_firstBestFit{
			parameters:
				appList=appList;
				vmList=vmList;
				userList=userList;
				dataCentersList=dataCentersList;
				slaList=slaList;
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
				userList=userList;
				slaList=slaList;
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

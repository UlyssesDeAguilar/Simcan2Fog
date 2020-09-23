package simcan2.Architecture.Network.DataCenterNetwork;

import simcan2.Architecture.Network.DataCenterNetwork.CommNetwork.CommNetwork;
import simcan2.Architecture.Network.DataCenterNetwork.StorageNetwork.StorageNetwork;
import simcan2.Architecture.Network.DataCenterNetwork.StorageNetwork.StorageNetwork;

//
// Network for DataCenters.
// It contains a communication network for inter-process communication and a storage network for accessing data.
//
module DataCenterNetwork{
        
    gates:
        input inComm[];
        output outComm[];	
        input inStorage[];
    	output outStorage[];
                
    submodules:
        
        commNet: CommNetwork{
        	gates:
        	    in[sizeof(inComm)];
        	    out[sizeof(outComm)];        
        }
        
        stoNet: StorageNetwork{
        	gates:
        	    in[sizeof(inStorage)];
        	    out[sizeof(outStorage)];        
        }        
        
    connections:
    	
    	// Connections between Comm gates and Comm network
    	for i=0..sizeof(inComm)-1 {
		   inComm++ --> commNet.in++;
		   outComm++ <-- commNet.out++;
		}
                                
    	// Link between Comm network and storage network
    	commNet.toStorage --> stoNet.fromComm;
    	commNet.fromStorage <-- stoNet.toComm;
    	
    	// Connections between Storage gates and Storage network
    	for i=0..sizeof(inStorage)-1 {
		   inStorage++ --> stoNet.in++;
		   outStorage++ <-- stoNet.out++;
		}
}


package simcan2.Architecture.Network.DataCentreNetwork;

import simcan2.Architecture.Network.DataCentreNetwork.CommNetwork.CommNetwork;
import simcan2.Architecture.Network.DataCentreNetwork.StorageNetwork.StorageNetwork;
import simcan2.Architecture.Network.DataCentreNetwork.StorageNetwork.StorageNetwork;

//
// Network for DataCentres.
// It contains a communication network for inter-process communication and a storage network for accessing data.
//
module DataCentreNetwork{
        
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


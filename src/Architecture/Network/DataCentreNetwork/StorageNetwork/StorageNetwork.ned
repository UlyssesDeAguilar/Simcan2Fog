package simcan2.Architecture.Network.DataCentreNetwork.StorageNetwork;
import simcan2.Core.cSIMCAN_Core;

//
// Storage network
//
simple StorageNetwork extends cSIMCAN_Core{
    
    @class(StorageNetwork);							//b
        
    gates:
        input in[];
        output out[];	
        input fromComm;
        output toComm;
}


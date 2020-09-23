package simcan2.Architecture.Network.DataCenterNetwork.CommNetwork;
import simcan2.Core.cSIMCAN_Core;

//
// Inter-process communication network
//
simple CommNetwork extends cSIMCAN_Core{
    
    @class(CommNetwork);							//b
                  
    gates:
        input in[];
        output out[];	
        input fromStorage;
        output toStorage;
}


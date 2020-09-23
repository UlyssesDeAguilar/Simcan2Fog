package simcan2.Architecture.Network.Switch;
import simcan2.Core.cSIMCAN_Core;

//
// Cloud switch for connecting CloudBlades and CloudBoards
//
simple Switch extends cSIMCAN_Core{
    
    parameters:
        string type;			// Type of cloud switch: "rack" or "board"        
        
        @class(Switch);							//b
        
    gates:
        input in[];	
        output out[];
        
    
}


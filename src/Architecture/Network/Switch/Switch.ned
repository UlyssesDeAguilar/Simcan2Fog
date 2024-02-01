package simcan2.Architecture.Network.Switch;
import simcan2.Core.cSIMCAN_Core;

//
// General switch, it assumes an address syntax of "x.x.x.x"  -> Transformed into a LocalAddress
// The level parameter indicates at which level it is located
//
simple Switch extends cSIMCAN_Core{
    
    parameters:
        @class(Switch);
        string type;			// Type of cloud switch: "rack" or "board"
        int level;				// Level at which the switch is located
        
    gates:
        inout upper;			// Gate for the upper level
        inout comm[];			// Communication gates    
}


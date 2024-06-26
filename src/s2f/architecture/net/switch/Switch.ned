package s2f.architecture.net.switch;

//
// General switch, it assumes an address syntax of "x.x.x.x"  -> Transformed into a LocalAddress
// The level parameter indicates at which level it is located
//
simple Switch{
    parameters:
        @class(Switch);
        @display("i=abstract/switch");
        //string type;			// Type of cloud switch: "rack" or "board"
        //int level;				// Level at which the switch is located
        
    gates:
        inout manager;			// Gate for the manager
        inout netStack;         // Gate for the network stack
        inout comm[];			// Communication gates    
}


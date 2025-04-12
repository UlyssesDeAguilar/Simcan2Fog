package s2f.architecture.net.switch;

//
// Assumes that the different hosts will advertise their position from
// the start, akin to ARP resolution addressing
//
simple Switch{
    parameters:
        @class(Switch);
        @display("i=abstract/switch");
    gates:
        inout comm[];			// Communication gates    
}


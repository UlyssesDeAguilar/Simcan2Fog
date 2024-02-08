package simcan2.Architecture.Nodes.Cloud;

import simcan2.Architecture.Nodes.Blade;
import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.DataCentreNetwork.*;

//
// Node Board that contains "numBlades" blades and one switch
//
module CloudBoard
{
    parameters:
        string address;			// The address of the Board inside a rack
        int level;
        int numBlades;
        string channelType;

        // Forward the level to the switch
        switch.level = level;

        @display("bgb=262,346");
    gates:
        inout comm;

    submodules:

        blade[numBlades]: CloudBlade {
            address = address + "." + string(index);	// Composing the address based on the index
            @display("p=193,158,c,120");
        }

        switch: Switch {
            @display("p=59,158");
        }
    connections:

        // Connections between the blades and the switch          
        for i=0..numBlades-1 {
            blade[i].comm <--> <channelType> like IDataCentreChannel <--> switch.comm++;
        }

		// Connections between the board and the switch
        comm <--> switch.upper;
}


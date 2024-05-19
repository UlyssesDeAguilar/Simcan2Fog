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
        @smlevel(0);
        int base;
        int numBlades;
        string channelType;

        @display("bgb=262,346");
    gates:
        inout comm;

    submodules:

        blade[numBlades]: CloudBlade {
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
        comm <--> switch.manager;
}


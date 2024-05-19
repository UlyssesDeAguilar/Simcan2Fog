package simcan2.Architecture.Nodes.Cloud;

import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.DataCentreNetwork.*;

//
// Rack that contains "numBoards" Boards and one switch.
//
// This FileConfigManager is shared by all nodes contained in this rack. 
//
module CloudRack
{
    parameters:
        @smlevel(1);
        string channelType;		// Type of the network inside the rack
        int numBoards;			// Number of Boards.
        int bladesPerBoard;		// Number of blades per Board.

        @display("bgb=286,383");
    gates:
        inout comm;

    submodules:

        board[numBoards]: CloudBoard {		// Boards
            parameters:
                channelType = channelType;
                numBlades = bladesPerBoard;
                @display("p=197,159,c,120");
        }

        switch: Switch {
            @display("p=64,159");
        }
    connections:
        // Connections between the boards and the switch          
        for i=0..numBoards-1 {
            board[i].comm <--> <channelType> like IDataCentreChannel <--> switch.comm++;
        }

		// Connections between the rack and the switch
        comm <--> switch.manager;
}



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
        string address;			// The address of the Rack inside a network 
        string channelType;		// Type of the network inside the rack
        int numBoards;			// Number of Boards.
        int bladesPerBoard;		// Number of blades per Board.
        int level;

        // Forward the level for the switch
        switch.level = level;
        @display("bgb=286,383");
    gates:
        inout comm;

    submodules:

        board[numBoards]: CloudBoard {		// Boards
            parameters:
                address = address + "." + string(index);	// Composing the address based on the index
                channelType = channelType;
                numBlades = bladesPerBoard;
                level = level + 1;
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
        comm <--> switch.upper;
}



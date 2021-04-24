package simcan2.Architecture.Nodes.Cloud;

import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.DataCentreNetwork.*;

//
// Rack that contains "numBoards" Boards and one switch.
//
// This FileConfigManager is shared by all nodes contained in this rack. 
//
module CloudRack{
    
    parameters:
        string channelType;		// Type of the network inside the rack
        int numBoards;			// Number of Boards.
        int bladesPerBoard;		// Number of blades per Board.

	gates:
	    input in;
	    output out;

    submodules:
        
        board[numBoards]: CloudBoard {		// Boards
            parameters:
                channelType = channelType;
                numBlades = bladesPerBoard;
        }
        
        switch: Switch{				// Switch
            parameters:
                type = "rack";
                
        	gates:
        		in[numBoards+1];	
      	   		out[numBoards+1];           
        }
        
                
 	connections:
 	    
        // Connections between the boards and the switch          
    	for i=0..numBoards-1 {
		   board[i].in <-- <channelType> like IDataCentreChannel  <-- switch.out++;
		   board[i].out --> <channelType> like IDataCentreChannel  --> switch.in++;
		}
		
		// Connections between the rack and the switch
       	in --> switch.in++;
       	out <-- switch.out++; 
}



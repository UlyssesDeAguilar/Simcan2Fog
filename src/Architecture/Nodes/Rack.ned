package simcan2.Architecture.Nodes;

import simcan2.Architecture.Nodes.NodeBoard;

//
// Rack that contains "numBoards" NodeBoards and a FileConfigManager.
//
// This FileConfigManager is shared by all nodes contained in this rack. 
//
module Rack{
    
    parameters:
        int numBoards;		// Number of Node Boards.
        int nodesPerBoard;	// Number of nodes per Board.

    submodules:
        nodeBoard[numBoards]: NodeBoard {
            parameters:
                numNodes = nodesPerBoard;
        }
}



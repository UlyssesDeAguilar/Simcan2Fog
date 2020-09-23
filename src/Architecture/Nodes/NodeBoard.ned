package simcan2.Architecture.Nodes;

import simcan2.Architecture.Nodes.Blade;

//
// Node Board that contains "numNode" nodes and a switch
//
module NodeBoard{
    
    parameters:
        int numNodes;

    submodules:
        node[numNodes]: Blade;
}


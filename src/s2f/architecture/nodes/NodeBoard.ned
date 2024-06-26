package s2f.Architecture.Nodes;

import s2f.Architecture.Nodes.Blade;

//
// Node Board that contains "numNode" nodes and a switch
//
module NodeBoard{
    
    parameters:
        int numNodes;

    submodules:
        node[numNodes]: Blade;
}


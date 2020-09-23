#include "Rack.h"

Rack::Rack(RackInfo* rackInfo){

    Node* currentNode;
    int i, totalNodes;

        // Set pointer to rackInfo
        this->rackInfo = rackInfo;

        // Total number of nodes
        totalNodes = rackInfo->getNumBoards() * rackInfo->getNodesPerBoard();

        for (i=0; i<totalNodes; i++){

            // Create a new node object
            currentNode = new Node (rackInfo->getNodeInfo()->getTotalMemoryGb(),
                                    rackInfo->getNodeInfo()->getTotalDiskGb(),
                                    rackInfo->getNodeInfo()->getNumCpUs(),
                                    true);

            // Add this node to the rack
            nodes.push_back(currentNode);
        }
}

int Rack::getNumNodes (){
    return nodes.size();
}

RackInfo* Rack::getRackInfo (){
    return rackInfo;
}

Node* Rack::getNode (int index){

    Node* requestedNode;

        // Init...
        requestedNode = nullptr;

        if ((index<0) || (index >= nodes.size()))
            throw omnetpp::cRuntimeError("Index out of bounds while accessing node %d in rack %s", index, rackInfo->toString().c_str());
        else
            requestedNode = nodes.at(index);

    return requestedNode;
}


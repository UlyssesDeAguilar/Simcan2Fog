#include "RackInfo.h"

RackInfo::RackInfo(std::string name, bool storage, int numBoards, int nodesPerBoard, NodeInfo* nodeInfo){
    this->name = name;
    this->storage = storage;
    this->numBoards = numBoards;
    this->nodesPerBoard = nodesPerBoard;
    this->nodeInfo = nodeInfo;
}

const std::string& RackInfo::getName() const {
    return name;
}

void RackInfo::setName(const std::string& name) {
    this->name = name;
}

NodeInfo* RackInfo::getNodeInfo() const {
    return nodeInfo;
}

void RackInfo::setNodeInfo(NodeInfo* nodeInfo) {
    this->nodeInfo = nodeInfo;
}

int RackInfo::getNodesPerBoard() const {
    return nodesPerBoard;
}

void RackInfo::setNodesPerBoard(int nodesPerBoard) {
    this->nodesPerBoard = nodesPerBoard;
}

int RackInfo::getNumBoards() const {
    return numBoards;
}

void RackInfo::setNumBoards(int numBoards) {
    this->numBoards = numBoards;
}

bool RackInfo::isStorage() const {
    return storage;
}

void RackInfo::setStorage(bool storage) {
    this->storage = storage;
}

std::string RackInfo::toString (){

    std::ostringstream text;
    std::string type;

        if (this->storage)
           type = " (Storage) ";
       else
           type = " (Computing) ";

        text << name << type;
        text << " - #Boards: "          << numBoards;
        text << " - #NodesPerBoard: "   << nodesPerBoard;

    return text.str();
}



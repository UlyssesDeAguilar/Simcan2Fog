#include "DataCenter.h"

DataCenter::DataCenter(std::string name){
    this->name = name;
}


DataCenter::~DataCenter(){
    computingRacks.clear();
    storageRacks.clear();
}


const std::string& DataCenter::getName() const {
    return name;
}

void DataCenter::addRack (Rack* rack, bool isStorage){

    if (!isStorage)
        computingRacks.push_back(rack);
    else
        storageRacks.push_back(rack);
}


Rack* DataCenter::getRack (int index, bool isStorage){

    Rack* rack;


        // Init...
        rack = nullptr;

        // Computing...
        if (!isStorage){

            if ((index<0) || (index >= computingRacks.size()))
                throw omnetpp::cRuntimeError("Error accessing a Rack out of bounds. Data-Center %s - Rack index %d - Computing", name.c_str(), index);
            else
                rack = computingRacks.at(index);
        }
        // Storage...
        else{

            if ((index<0) || (index >= storageRacks.size()))
                throw omnetpp::cRuntimeError("Error accessing a Rack out of bounds. Data-Center %s - Rack index %d - Storage", name.c_str(), index);
            else
                rack = storageRacks.at(index);
        }

    return rack;
}


int DataCenter::getNumRacks (bool isStorage){

    int numRacks;

        if (!isStorage)
            numRacks = computingRacks.size();
        else
            numRacks = storageRacks.size();

    return numRacks;
}


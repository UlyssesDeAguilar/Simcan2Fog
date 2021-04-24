#include "DataCentre.h"

DataCentre::DataCentre(std::string name){
    this->name = name;
}


DataCentre::~DataCentre(){
    computingRacks.clear();
    storageRacks.clear();
}


const std::string& DataCentre::getName() const {
    return name;
}

void DataCentre::addRack (Rack* rack, bool isStorage){

    if (!isStorage)
        computingRacks.push_back(rack);
    else
        storageRacks.push_back(rack);
}


Rack* DataCentre::getRack (int index, bool isStorage){

    Rack* rack;


        // Init...
        rack = nullptr;

        // Computing...
        if (!isStorage){

            if ((index<0) || (index >= computingRacks.size()))
                throw omnetpp::cRuntimeError("Error accessing a Rack out of bounds. Data-Centre %s - Rack index %d - Computing", name.c_str(), index);
            else
                rack = computingRacks.at(index);
        }
        // Storage...
        else{

            if ((index<0) || (index >= storageRacks.size()))
                throw omnetpp::cRuntimeError("Error accessing a Rack out of bounds. Data-Centre %s - Rack index %d - Storage", name.c_str(), index);
            else
                rack = storageRacks.at(index);
        }

    return rack;
}


int DataCentre::getNumRacks (bool isStorage){

    int numRacks;

        if (!isStorage)
            numRacks = computingRacks.size();
        else
            numRacks = storageRacks.size();

    return numRacks;
}


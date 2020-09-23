#include "RackInstances.h"

RackInstances::RackInstances(RackInfo* rackInfo){
    this->rackInfo = rackInfo;
}

void RackInstances::insertNewRack (Rack* newRack){
    racks.push_back(newRack);
}

int RackInstances::getNumRacks(){
    return racks.size();
}


std::string RackInstances::toString (bool showNodeInfoFeatures){

    std::ostringstream text;
    int i;

        text << std::endl << "Racks Instances: " << racks.size() << std::endl;

        for (i=0; i<racks.size(); i++){

            //text << "Rack[" << i << "] --> " << racks.at(i)->toString(true) << std::endl;
        }

        if (showNodeInfoFeatures)
            text << rackInfo->toString();

    return text.str();
}



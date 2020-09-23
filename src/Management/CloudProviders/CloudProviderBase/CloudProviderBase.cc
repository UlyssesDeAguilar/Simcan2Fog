#include "CloudProviderBase.h"

CloudProviderBase::~CloudProviderBase(){

//    computingNodeTypes.clear();
//    storageNodeTypes.clear();
//    computingRackTypes.clear();
//    storageRackTypes.clear();
    dataCentersBase.clear();
}


void CloudProviderBase::initialize(){

    int result;
    int numGates, i;

        // Init super-class
        CloudManagerBase::initialize();

            // Get parameters from module
            showDataCenters = par ("showDataCenters");

            // Number of gates
            numGates = gateSize ("fromDataCenter");

            // Init the size of the cGate vectors
            fromDataCenterGates = new cGate* [numGates];
            toDataCenterGates = new cGate* [numGates];

            // Init the cGates vector
            for (i=0; i<numGates; i++){

               // Get cGate object
               fromDataCenterGates [i] = gate ("fromDataCenter", i);
               toDataCenterGates [i] = gate ("toDataCenter", i);

               // Checking connections
               if (!toDataCenterGates[i]->isConnected()){
                   error ("toDataCenterGates %d is not connected", i);
               }
            }

            // Gates to connect the user generator
            fromUserGeneratorGate = gate ("fromUserGenerator");
            toUserGeneratorGate = gate ("toUserGenerator");

            // Parse data-center list
            result = parseDataCentersList();

            // Something goes wrong...
            if (result == SC_ERROR){
                error ("Error while parsing data-centers list");
            }
            else if (showDataCenters){
                EV_DEBUG << dataCentersToString ();
            }
}


cGate* CloudProviderBase::getOutGate (cMessage *msg){

    cGate* outGate;

       // Init...
       outGate = nullptr;

        // If msg arrives from DataCenter
        if (msg->arrivedOn("fromDataCenterGates")){
           outGate = gate ("toDataCenterGates", msg->getArrivalGate()->getIndex());
        }

        // If msg arrives from user generator
        else if (msg->getArrivalGate()==fromUserGeneratorGate){
            outGate = toUserGeneratorGate;
        }

        // Msg arrives from an unknown gate
        else
            error ("Message received from an unknown gate [%s]", msg->getName());

    return outGate;
}


int CloudProviderBase::parseDataCentersList (){
    int result;
    const char *dataCentersListChr;

    dataCentersListChr= par ("dataCentersList");
    DataCenterListParser dataCenterParser(dataCentersListChr);
    result = dataCenterParser.parse();
    if (result == SC_OK) {
        dataCentersBase = dataCenterParser.getResult();
    }
    return result;
}


std::string CloudProviderBase::dataCentersToString (){

    std::ostringstream info;
    int i, currentRack;

        // Main text for the applications of this manager
        info << std::endl << dataCentersBase.size() << " Data-Centers parsed from CloudProviderBase in " << getFullPath() << endl << endl;

        for (i=0; i<dataCentersBase.size(); i++){

            // Name of the data-center
            info << "\tData-Center[" << i << "]  --> " << dataCentersBase.at(i)->getName() << "  -  ";

            // Number of racks
            info << dataCentersBase.at(i)->getNumRacks(false) << " computing racks  -  ";
            info << dataCentersBase.at(i)->getNumRacks(true) << " storage racks" << endl;

            // Features of each rack
            for (currentRack=0; currentRack < dataCentersBase.at(i)->getNumRacks(false); currentRack++){
                info << "\t  + Rack[" << currentRack << "]:" << dataCentersBase.at(i)->getRack(currentRack, false)->getRackInfo()->toString() << endl;
                info << "\t\t - Node config: " << dataCentersBase.at(i)->getRack(currentRack, false)->getRackInfo()->getNodeInfo()->toString() << endl;
            }

            // Features of each rack
            for (currentRack=0; currentRack < dataCentersBase.at(i)->getNumRacks(true); currentRack++){
                info << "\t  + Rack[" << currentRack + dataCentersBase.at(i)->getNumRacks(false) << "]:" << dataCentersBase.at(i)->getRack(currentRack, true)->getRackInfo()->toString() << endl;
                info << "\t\t - Node config: " << dataCentersBase.at(i)->getRack(currentRack, true)->getRackInfo()->getNodeInfo()->toString() << endl;
            }

            info << endl;
        }

        info << "---------------- End of parsed Data-Centers in " << getFullPath() << " ----------------" << endl;

    return info.str();
}


//NodeInfo* CloudProviderBase::findNodeInfo (std::string nodeName, bool isStorage){
//
//    std::vector<NodeInfo*>::iterator it;
//    std::vector<NodeInfo*>::iterator itEnd;
//    NodeInfo* result;
//    bool found;
//
//        // Init
//        found = false;
//        result = nullptr;
//
//        // Set iterators
//        if (!isStorage){
//            it = computingNodeTypes.begin();
//            itEnd = computingNodeTypes.end();
//        }
//        else{
//            it = storageNodeTypes.begin();
//            itEnd = storageNodeTypes.end();
//        }
//
//            // Search...
//            while((!found) && (it != itEnd)){
//
//                if ((*it)->getName() == nodeName){
//                    found = true;
//                    result = *it;
//                }
//                else
//                    it++;
//            }
//
//    return result;
//}
//
//
//RackInfo* CloudProviderBase::findRackInfo (std::string rackName, bool isStorage){
//
//    std::vector<RackInfo*>::iterator it;
//    std::vector<RackInfo*>::iterator itEnd;
//    RackInfo* result;
//    bool found;
//
//        // Init
//        found = false;
//        result = nullptr;
//
//        // Set iterators
//        if (!isStorage){
//            it = computingRackTypes.begin();
//            itEnd = computingRackTypes.end();
//        }
//        else{
//            it = storageRackTypes.begin();
//            itEnd = storageRackTypes.end();
//        }
//
//            // Search...
//            while((!found) && (it != itEnd)){
//
//                if ((*it)->getName() == rackName){
//                    found = true;
//                    result = *it;
//                }
//                else
//                    it++;
//            }
//
//    return result;
//}


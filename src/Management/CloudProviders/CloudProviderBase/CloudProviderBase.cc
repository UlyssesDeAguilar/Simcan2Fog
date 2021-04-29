#include "CloudProviderBase.h"

CloudProviderBase::~CloudProviderBase(){

//    computingNodeTypes.clear();
//    storageNodeTypes.clear();
//    computingRackTypes.clear();
//    storageRackTypes.clear();
    dataCentresBase.clear();
}


void CloudProviderBase::initialize(){

    int result;
    int numGates, i;

        // Init super-class
        CloudManagerBase::initialize();

            // Get parameters from module
            showDataCentres = par ("showDataCentres");

            // Number of gates
            numGates = gateSize ("fromDataCentre");

            // Init the size of the cGate vectors
            fromDataCentreGates = new cGate* [numGates];
            toDataCentreGates = new cGate* [numGates];
            dataCentreManagers = new DataCentreManagerBase* [numGates];

            // Init the cGates vector
            for (i=0; i<numGates; i++){

               // Get cGate object
               fromDataCentreGates [i] = gate ("fromDataCentre", i);
               toDataCentreGates [i] = gate ("toDataCentre", i);

               // Checking connections
               if (!toDataCentreGates[i]->isConnected()){
                   error ("toDataCentreGates %d is not connected", i);
               }

               dataCentreManagers [i] = dynamic_cast<DataCentreManagerBase*>(getParentModule()->getSubmodule("dc_DataCentre", i)->getSubmodule("dcManager"));
            }

            // Gates to connect the user generator
            fromUserGeneratorGate = gate ("fromUserGenerator");
            toUserGeneratorGate = gate ("toUserGenerator");

            // Parse data-centre list

            //TODO: Meta-data only
            result = parseDataCentresList();

            // Something goes wrong...
            if (result == SC_ERROR){
                error ("Error while parsing data-centres list");
            }
            else if (showDataCentres){
                EV_DEBUG << dataCentresToString ();
            }
}


cGate* CloudProviderBase::getOutGate (cMessage *msg){

    cGate* outGate;

       // Init...
       outGate = nullptr;

        // If msg arrives from DataCentre
        if (msg->arrivedOn("fromDataCentreGates")){
           outGate = gate ("toDataCentreGates", msg->getArrivalGate()->getIndex());
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


int CloudProviderBase::parseDataCentresList (){
    int result;
    const char *dataCentresListChr;

    dataCentresListChr= par ("dataCentresList");
    DataCentreListParser dataCentreParser(dataCentresListChr);
    result = dataCentreParser.parse();
    if (result == SC_OK) {
        dataCentresBase = dataCentreParser.getResult();
    }
    return result;
}


std::string CloudProviderBase::dataCentresToString (){

    std::ostringstream info;
    int i, currentRack;

        // Main text for the applications of this manager
        info << std::endl << dataCentresBase.size() << " Data-Centres parsed from CloudProviderBase in " << getFullPath() << endl << endl;

        for (i=0; i<dataCentresBase.size(); i++){

            // Name of the data-centre
            info << "\tData-Centre[" << i << "]  --> " << dataCentresBase.at(i)->getName() << "  -  ";

            // Number of racks
            info << dataCentresBase.at(i)->getNumRacks(false) << " computing racks  -  ";
            info << dataCentresBase.at(i)->getNumRacks(true) << " storage racks" << endl;

            // Features of each rack
            for (currentRack=0; currentRack < dataCentresBase.at(i)->getNumRacks(false); currentRack++){
                info << "\t  + Rack[" << currentRack << "]:" << dataCentresBase.at(i)->getRack(currentRack, false)->getRackInfo()->toString() << endl;
                info << "\t\t - Node config: " << dataCentresBase.at(i)->getRack(currentRack, false)->getRackInfo()->getNodeInfo()->toString() << endl;
            }

            // Features of each rack
            for (currentRack=0; currentRack < dataCentresBase.at(i)->getNumRacks(true); currentRack++){
                info << "\t  + Rack[" << currentRack + dataCentresBase.at(i)->getNumRacks(false) << "]:" << dataCentresBase.at(i)->getRack(currentRack, true)->getRackInfo()->toString() << endl;
                info << "\t\t - Node config: " << dataCentresBase.at(i)->getRack(currentRack, true)->getRackInfo()->getNodeInfo()->toString() << endl;
            }

            info << endl;
        }

        info << "---------------- End of parsed Data-Centres in " << getFullPath() << " ----------------" << endl;

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


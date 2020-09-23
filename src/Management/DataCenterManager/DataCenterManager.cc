#include "DataCenterManager.h"

Define_Module(DataCenterManager);

DataCenterManager::~DataCenterManager(){
}

void DataCenterManager::initialize(){

    int result;

    // Init super-class
    CloudManagerBase::initialize();

        // Get parameters from module
        showDataCenterConfig = par ("showDataCenterConfig");

        // Get gates
        inGate = gate ("in");
        outGate = gate ("out");

        // Parse data-center list
        result = parseDataCenterConfig();

        // Something goes wrong...
        if (result == SC_ERROR){
            error ("Error while parsing data-centers list");
        }
        else if (showDataCenterConfig){
            EV_DEBUG << dataCenterToString ();
        }
}


int DataCenterManager::parseDataCenterConfig (){

    return 0;
}


std::string DataCenterManager::dataCenterToString (){

    std::ostringstream info;
    int i;

        info << std::endl;

    return info.str();
}


cGate* DataCenterManager::getOutGate (cMessage *msg){

    cGate* nextGate;

        // Init...
        nextGate = nullptr;

        // If msg arrives from the Hypervisor
        if (msg->getArrivalGate()==inGate){
            nextGate = outGate;
        }

        // Msg arrives from an unknown gate
        else
            error ("Message received from an unknown gate [%s]", msg->getName());


    return nextGate;
}


void DataCenterManager::processSelfMessage (cMessage *msg){

}

void DataCenterManager::processRequestMessage (SIMCAN_Message *sm){

}

void DataCenterManager::processResponseMessage (SIMCAN_Message *sm){

}

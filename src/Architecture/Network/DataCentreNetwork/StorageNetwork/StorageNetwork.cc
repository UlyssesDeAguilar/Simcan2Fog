#include "../../DataCentreNetwork/StorageNetwork/StorageNetwork.h"

Define_Module(StorageNetwork);

StorageNetwork::~StorageNetwork(){
}

void StorageNetwork::initialize(){

    int numGates, i;

        // Init the super-class
        cSIMCAN_Core::initialize();

        // Get the number of input and output gates
        numGates = gateSize ("in");

        // Init the cGates vector for inputs
        inputGates = new cGate* [numGates];
        outputGates = new cGate* [numGates];

        for (i=0; i<numGates; i++){
            inputGates [i] = gate ("in", i);
            outputGates [i] = gate ("out", i);

            if (!(outputGates[i]->isConnected())){
                error ("Gate is not connected");
            }
        }

        fromCommGate = gate("fromComm");
        toCommGate = gate("toComm");
}


void StorageNetwork::finish(){

    // Finish the super-class
    cSIMCAN_Core::finish();
}


cGate* StorageNetwork::getOutGate (cMessage *msg){

    cGate* outGate;

        // Init...
        outGate = nullptr;

            // If msg arrives
            if (msg->arrivedOn("in")){
                outGate = gate ("out", msg->getArrivalGate()->getIndex());
            }

            else if (msg->getArrivalGate() == fromCommGate){
                outGate = toCommGate;
            }

            // Msg arrives from an unknown gate
            else
                error ("Message received from an unknown gate [%s]", msg->getName());

   return outGate;
}


void StorageNetwork::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void StorageNetwork::processRequestMessage (SIMCAN_Message *sm){

    // TODO: Finish behaviour!

}


void StorageNetwork::processResponseMessage (SIMCAN_Message *sm){

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    sendResponseMessage(sm);
}

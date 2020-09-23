#include "CommNetwork.h"

Define_Module(CommNetwork);

CommNetwork::~CommNetwork(){
}

void CommNetwork::initialize(){

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

        fromStorageGate = gate("fromStorage");
        toStorageGate = gate("toStorage");
}


void CommNetwork::finish(){

    // Finish the super-class
    cSIMCAN_Core::finish();
}


cGate* CommNetwork::getOutGate (cMessage *msg){

    cGate* outGate;

        // Init...
        outGate = nullptr;

            // If msg arrives
            if (msg->arrivedOn("in")){
                outGate = gate ("out", msg->getArrivalGate()->getIndex());
            }

            // Msg arrives from an unknown gate
            else
                error ("Message received from an unknown gate [%s]", msg->getName());

   return outGate;
}


void CommNetwork::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void CommNetwork::processRequestMessage (SIMCAN_Message *sm){

    // TODO: Finish behaviour!

}


void CommNetwork::processResponseMessage (SIMCAN_Message *sm){

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    sendResponseMessage(sm);
}

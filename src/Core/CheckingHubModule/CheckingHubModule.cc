#include "CheckingHubModule.h"

Define_Module(CheckingHubModule);

CheckingHubModule::~CheckingHubModule(){
}

void CheckingHubModule::initialize(){

    int numInputGates, numOutputGates, i;

        // Init the super-class
        cSIMCAN_Core::initialize();

        // Init module parameters
        staticAppAssignment = par ("staticAppAssignment");

        // Get the number of input and output gates
        numInputGates = gateSize ("fromInput");
        numOutputGates = gateSize ("fromOutput");

        // Init the cGates vector for inputs
        fromInputGates = new cGate* [numInputGates];
        toInputGates = new cGate* [numInputGates];

        for (i=0; i<numInputGates; i++){
            fromInputGates [i] = gate ("fromInput", i);
            toInputGates [i] = gate ("toInput", i);

            if (!(toInputGates[i]->isConnected())){
                error ("Gate is not connected to input");
            }
        }

        // Init the cGates vector for outputs
        fromOutputGates = new cGate* [numOutputGates];
        toOutputGates = new cGate* [numOutputGates];

        for (i=0; i<numOutputGates; i++){
            fromOutputGates [i] = gate ("fromOutput", i);
            toOutputGates [i] = gate ("toOutput", i);

            if (!(toOutputGates[i]->isConnected())){
               error ("Gate is not connected to output");
            }
        }

}


void CheckingHubModule::finish(){

    // Finish the super-class
    cSIMCAN_Core::finish();
}


cGate* CheckingHubModule::getOutGate (cMessage *msg){

    cGate* outGate;

        // Init...
        outGate = nullptr;

            // If msg arrives from Input gates
            if (msg->arrivedOn("fromInput")){
                outGate = gate ("toInput", msg->getArrivalGate()->getIndex());
            }

            // If msg arrives from Output gates
            else if (msg->arrivedOn("fromOutput")){
                outGate = gate ("toOutput", msg->getArrivalGate()->getIndex());
            }

            // Msg arrives from an unknown gate
            else
                error ("Message received from an unknown gate [%s]", msg->getName());

   return outGate;
}


void CheckingHubModule::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void CheckingHubModule::processRequestMessage (SIMCAN_Message *sm){

    // Error locating output gates...
    if (gateSize("toOutput") == 0){
        error ("toOutput gates vector has size=0");
    }

    // Output gates contain only 1 connection
    else if (gateSize("toOutput") == 1){

        // Debug (Debug)
        EV_DEBUG << "(processRequestMessage) Sending request message. Only one output gate -> toOutputGates[0]."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

        sendRequestMessage (sm, toOutputGates[0]);
    }

    // Output gates contains >1 connections
    else{

        // Checks for the next module
        if (sm->getNextModuleIndex() == SM_UnsetValue){
            error ("Unset value for nextModuleIndex... and there are several output gates. %s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
        }

        // Next module index is out of bounds...
        else if ((sm->getNextModuleIndex() < 0) || (sm->getNextModuleIndex() >= gateSize("toOutput"))){
            error ("nextModuleIndex %d is out of bounds [%d]", sm->getNextModuleIndex(), gateSize("toOutput"));
        }

        // Everything is OK... send the message! ;)
        else{

            // Debug (Debug)
            EV_DEBUG << "(processRequestMessage) Sending request message. There are several output gates -> toOutputGates["<< sm->getNextModuleIndex() << "]" << endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

            sendRequestMessage (sm, toOutputGates[sm->getNextModuleIndex()]);
        }
    }
}


void CheckingHubModule::processResponseMessage (SIMCAN_Message *sm){

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

    sendResponseMessage(sm);
}

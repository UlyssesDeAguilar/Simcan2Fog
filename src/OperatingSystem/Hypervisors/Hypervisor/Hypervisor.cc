#include "Hypervisor.h"

Define_Module (Hypervisor);


Hypervisor::~Hypervisor(){
}

void Hypervisor::initialize(){

	int i, numCpuGates, numAppGates;

	    // Init the super-class
	    cSIMCAN_Core::initialize();

	        // Init module parameters
            isVirtualHardware = par ("isVirtualHardware");
            maxVMs = (unsigned int) par ("maxVMs");

            // Get the number of gates for each vector
            numAppGates = gateSize ("fromApps");
            numCpuGates = gateSize ("fromCpuScheduler");

            // Init the size of the cGate vectors
            fromAppsGates = new cGate* [numAppGates];
            toAppsGates = new cGate* [numAppGates];

            // Init the cGates vector for Applications
            for (i=0; i<numAppGates; i++){
                fromAppsGates [i] = gate ("fromApps", i);
                toAppsGates [i] = gate ("toApps", i);

                // Checking connections
                if (!toAppsGates[i]->isConnected()){
                    EV_ERROR << "toAppsGates[" << i << "] is not connected";
                    error ("toAppsGates is not connected");
                }
            }

            // Init the cGates vector for CPU scheduler
            fromCPUGates = new cGate* [numCpuGates];
            toCPUGates = new cGate* [numCpuGates];

            for (i=0; i<numCpuGates; i++){
                fromCPUGates [i] = gate ("fromCpuScheduler", i);
                toCPUGates [i] = gate ("toCpuScheduler", i);
            }
}


void Hypervisor::finish(){

	// Finish the super-class
	cSIMCAN_Core::finish();
}


cGate* Hypervisor::getOutGate (cMessage *msg){

    cGate* outGate;

           // Init...
           outGate = nullptr;

            // If msg arrives from Application gates
            if (msg->arrivedOn("fromApps")){
               outGate = gate ("toApps", msg->getArrivalGate()->getIndex());
            }

            // If msg arrives from CPU scheduler
            else if (msg->arrivedOn("fromCpuScheduler")){
                error ("This module cannot receive requests from the CPU system!");
            }

            // Msg arrives from an unknown gate
            else
                error ("Message received from an unknown gate [%s]", msg->getName());

    return outGate;
}


void Hypervisor::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void Hypervisor::processRequestMessage (SIMCAN_Message *sm){

    // Message came from CPU
    if (sm->arrivedOn("fromCpuScheduler")){
        error ("This module cannot receive request messages from CPU!!!");
    }

    // Message came from applications
    else if (sm->arrivedOn("fromApps")){

        EV_DEBUG << "(processRequestMessage) Message arrives from applications."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

        // CPU operation?
        if (sm->getOperation () == SM_ExecCpu){

            // Virtual HW! There is only 1 CPU scheduler
            if (!isVirtualHardware){

                // Debug
                EV_INFO << "Sending request message to CPU."<< endl << sm->contentsToString(showMessageContents, showMessageTrace).c_str() << endl;

                sendRequestMessage (sm, toCPUGates[0]);
            }
            else{
                //TODO: Manage users/VMs to redirec requests to the corresponding CPU scheduler
            }
        }

        // Unknown operation!
        else{
            error ("Unknown operation:%d", sm->getOperation ());
        }


//        else
//              // I/O operation?
//              if ((sm->getOperation () == SM_OPEN_FILE)   ||
//                  (sm->getOperation () == SM_CLOSE_FILE)  ||
//                  (sm->getOperation () == SM_READ_FILE)   ||
//                  (sm->getOperation () == SM_WRITE_FILE)  ||
//                  (sm->getOperation () == SM_CREATE_FILE) ||
//                  (sm->getOperation () == SM_DELETE_FILE)){
//
//                      // Remote operation? to NET
//                      if (sm->getRemoteOperation()){
//
//        //                    if (DEBUG_Service_Redirector)
//        //                        showDebugMessage ("Local request redirected to NET. %s",
//        //                                            sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                          sendRequestMessage (sm, toNetGate);
//                      }
//
//                      // Local operation? to local FS
//                      else{
//
//        //                    if (DEBUG_Service_Redirector)
//        //                        showDebugMessage ("Local request redirected to I/O. %s",
//        //                                            sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//        //
//                          sendRequestMessage (sm, toMemoryGate);
//                      }
//              }
//
//              // MPI operation?
//              else if ((sm->getOperation () == MPI_SEND) ||
//                       (sm->getOperation () == MPI_RECV) ||
//                       (sm->getOperation () == MPI_BARRIER_UP)   ||
//                       (sm->getOperation () == MPI_BARRIER_DOWN) ||
//                       (sm->getOperation () == MPI_BCAST)   ||
//                       (sm->getOperation () == MPI_SCATTER) ||
//                       (sm->getOperation () == MPI_GATHER)){
//
//                  sendRequestMessage (sm, toNetGate);
//              }
//
//
//
//
//              // MEM operation?
//              else if ((sm->getOperation () == SM_MEM_ALLOCATE) ||
//                       (sm->getOperation () == SM_MEM_RELEASE)){
//
//        //            if (DEBUG_Service_Redirector)
//        //                showDebugMessage ("Local request redirected to MEM. %s", sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                  sendRequestMessage (sm, toMemoryGate);
//              }
//
//
//              // Net operation?
//              else if ((sm->getOperation () == SM_CREATE_CONNECTION) ||
//                      (sm->getOperation () == SM_LISTEN_CONNECTION) ||
//                      (sm->getOperation () == SM_SEND_DATA_NET)){
//
//        //            if (DEBUG_Service_Redirector)
//        //                showDebugMessage ("Local request redirected to NET. %s", sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                  sendRequestMessage (sm, toNetGate);
//              }

    }



//	// Msg cames from Network
//	if (sm->getArrivalGate() == fromNetGate){
//
//		if (DEBUG_Service_Redirector)
//			showDebugMessage ("Incomming request from NET... sending to App[%d]. %s",
//								sm->getNextModuleIndex(),
//								sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//		sendRequestMessage (sm, toAppGates[sm->getNextModuleIndex()]);
//	}

	
	
	
	
//	// Msg cames from Memory
//	else if (sm->getArrivalGate() == fromMemoryGate){
//
//		if (DEBUG_Service_Redirector)
//			showDebugMessage ("Incomming request from OS... sending to App[%d]. %s",
//								sm->getNextModuleIndex(),
//								sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//		sendRequestMessage (sm, toAppGates[sm->getNextModuleIndex()]);
//	}

}


void Hypervisor::processResponseMessage (SIMCAN_Message *sm){

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;
		
	// Send back the message
	sendResponseMessage (sm);
}


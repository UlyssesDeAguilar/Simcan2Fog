#include "CpuCore.h"

Define_Module (CpuCore);

CpuCore::~CpuCore(){
}


void CpuCore::initialize(){

    // Init the super-class
    cSIMCAN_Core::initialize();

        // Init module parameters
        speed = par ("speed");
        tick = par ("tick");
        ipt = speed * tick.dbl();

        // Init message used for timers
        pendingMessage = nullptr;

        // Latency message
        latencyMessage = new cMessage (Timer_Latency.c_str());

        // Init cGates
        inGate = gate ("in");
        outGate = gate ("out");

        // Check connections
        if (!outGate->isConnected()){
            EV_ERROR << "outGate is not connected";
            error ("outGate is not connected");
        }

        // Show additional data
        if (showInitValues)
            EV_DEBUG << "Speed:" << speed << " - tick:" << tick << " - ipt:" << ipt << endl;
}


void CpuCore::finish(){

	// Finish the super-class
	cSIMCAN_Core::finish();
}


cGate* CpuCore::getOutGate (cMessage *msg){

    cGate* gate;

        // Init...
        gate = nullptr;

        // If msg arrives from CPU scheduler
        if (msg->getArrivalGate()==inGate){
            gate = outGate;
        }
        else
            error ("Message received from an unknown gate [%s]", msg->getName());

	return gate;
}


void CpuCore::processSelfMessage (cMessage *msg){
	
    SM_CPU_Message *sm_cpu;
	
		// Latency message...
		if (!strcmp (msg->getName(), Timer_Latency.c_str())){
	
			// Cast!
			sm_cpu = check_and_cast<SM_CPU_Message *>(pendingMessage);


			EV_DEBUG << "(processSelfMessage) Tick finished."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

			// Check if this request is completed OR it has completed its quantum
			if ((sm_cpu->getIsCompleted()) || (sm_cpu->getQuantum()==0)){

			    EV_INFO << "CPU request done! Sending it back to CPU scheduler" << endl;

                // Init pending message...
                pendingMessage = nullptr;

                // Response message
                sm_cpu->setIsResponse(true);

                // Send message back!
                sendResponseMessage (sm_cpu);
			}

			// Current request is not completed
			else{

			    // Execute current request
                if (sm_cpu->getUseTime()){
                    EV_INFO << "CPU request unfinished! Executing another tick (CPU request in time unit)" << endl;
                    sm_cpu->executeTime(tick);
                }
                else{
                    EV_INFO << "CPU request unfinished! Executing another tick (CPU request in MIs unit)" << endl;
                    sm_cpu->executeMIs(ipt);
                }

			    // Link pending message
                pendingMessage = sm_cpu;

                // Execute a tick
                scheduleAt (simTime()+tick, latencyMessage);
			}
		}
	
		else{
			error ("Unknown self message:%s", msg->getName());
		}
}


void CpuCore::processRequestMessage (SIMCAN_Message *sm){
	
    SM_CPU_Message *sm_cpu;

	    // Casting
		sm_cpu = check_and_cast<SM_CPU_Message *>(sm);

		// Link pending message
		pendingMessage = sm_cpu;

		EV_DEBUG << "(processRequestMessage) Processing request."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

		// Check the quantum is >0
		if ((sm_cpu->getQuantum() <= 0) && (sm_cpu->getQuantum() != SM_CpuInfiniteQuantum)){
		    error ("CPU request with quantum=0. %s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
		}

		// There is at least 1 execution tick!
		else{

            // Execute current request
            if (sm_cpu->getUseTime()){
                EV_INFO << "Executing one tick (CPU request in time unit)" << endl;
                sm_cpu->executeTime(tick);
            }
            else{
                EV_INFO << "Executing one tick (CPU request in MIs unit)" << endl;
                sm_cpu->executeMIs(ipt);
            }

            // Execute a tick
            scheduleAt (simTime()+tick, latencyMessage);
		}
}


void CpuCore::processResponseMessage (SIMCAN_Message *sm){
    error ("This module cannot process self messages:%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
}

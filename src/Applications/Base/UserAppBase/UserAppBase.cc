#include "UserAppBase.h"


UserAppBase::~UserAppBase(){
	
	connections.clear();
}


void UserAppBase::initialize(){
	
	// Init the super-class
	cSIMCAN_Core::initialize();

        // Init module parameters
	    startDelay = par ("startDelay");
	    connectionDelay = par ("connectionDelay");
        isDistributedApp = par ("isDistributedApp");
        myRank = (unsigned int ) par ("myRank");
        const char *newTestID = par ("testID");
        testID = newTestID;
        const char *newAppInstance = par ("appInstance");
        appInstance = newAppInstance;
        debugUserAppBase = par ("debugUserAppBase");


        // Init cGates
        inGate = gate ("in");
        outGate = gate ("out");

        // Check connections
        if (!outGate->getNextGate()->isConnected()){
            error ("outGate is not connected");
        }
}


void UserAppBase::finish(){

	// Finish the super-class
	cSIMCAN_Core::finish();
}


cGate* UserAppBase::getOutGate (cMessage *msg){

    cGate* gate;

        // Init...
        gate = nullptr;

        // If msg arrives from the Operating System
        if (msg->getArrivalGate()==inGate){
            gate = outGate;
        }

	// If gate not found!
	return gate;
}


// ----------------- CPU calls ----------------- //

void UserAppBase::SIMCAN_request_cpu (double MIs){
	
	SM_CPU_Message *sm_cpu;		// Request message!
	
		// Creates the message
		sm_cpu = new SM_CPU_Message ();
		sm_cpu->setOperation (SM_ExecCpu);
		
		// Set the corresponding parameters
		sm_cpu->setAppInstance(appInstance);
		sm_cpu->setUseMis(true);
		sm_cpu->setMisToExecute(MIs);

		// Update message length
		sm_cpu->updateLength ();

		sm_cpu->setNextModuleIndex(getParentModule()->getParentModule()->getIndex());

		// Debug (Trace)
        if (debugUserAppBase)
            EV_TRACE << "(SIMCAN_request_cpu): Message ready to perform a CPU request."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

		// Send the request to the Operating System
		sendRequestMessage (sm_cpu, outGate);
}

void UserAppBase::SIMCAN_abort_request_cpu (){

    SM_CPU_Message *sm_cpu;     // Request message!

        // Creates the message
        sm_cpu = new SM_CPU_Message ();
        sm_cpu->setOperation (SM_AbortCpu);

        sm_cpu->setAppInstance(appInstance);

        sm_cpu->setNextModuleIndex(getParentModule()->getParentModule()->getIndex());

        // Debug (Trace)
        if (debugUserAppBase)
            EV_TRACE << "(SIMCAN_request_cpu): Message ready to abort a CPU request."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

        // Send the request to the Operating System
        sendRequestMessage (sm_cpu, outGate);
}

void UserAppBase::SIMCAN_request_cpuTime (simtime_t cpuTime){
	
    SM_CPU_Message *sm_cpu;		// Request message!
	
		// Creates the message
		sm_cpu = new SM_CPU_Message ();
		sm_cpu->setOperation (SM_ExecCpu);
		
		// Set the corresponding parameters
		sm_cpu->setAppInstance(appInstance);
		sm_cpu->setUseTime (true);
		sm_cpu->setCpuTime (cpuTime);
		
		// Update message length
		sm_cpu->updateLength ();

		// Debug (Trace)
        if (debugUserAppBase)
            EV_TRACE << "(SIMCAN_request_cpuTime): Message ready to perform a CPU request."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

		// Send the request to the Operating System
		sendRequestMessage (sm_cpu, outGate);
}



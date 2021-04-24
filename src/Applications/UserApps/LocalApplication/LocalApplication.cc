#include "LocalApplication.h"
#define MAX_FILE_SIZE 2000000000

Define_Module(LocalApplication);

LocalApplication::~LocalApplication(){

}

void LocalApplication::initialize(){

    bool initialized = par ("initialized");


		// Init the super-class
		UserAppBase::initialize();

		selfMessage= nullptr;

		connections = vector <connector> ();

		// App Module parameters
		inputDataSize  = (int) par ("inputDataSize");
		outputDataSize  = (int) par ("outputDataSize");
		MIs  = par ("MIs");

		iterations  = par ("iterations");
		

		const char *newInputFile = par ("inputFile");
		inputFile = newInputFile;

		const char *newOutputFile = par ("outputFile");
		outputFile = newOutputFile;

		// Service times		
		total_service_IO = total_service_CPU = 0.0;
		startServiceIO = endServiceIO = 0.0;
		endServiceCPU = startServiceCPU = 0.0;
		simEndTime = simStartTime = 0.0;
		runEndTime = runStartTime = 0.0;
		readOffset = writeOffset = 0;
		
		// Boolean variables
		executeCPU = executeWrite = false;
		executeRead = true;

		pDataCentreManager = dynamic_cast<DataCentreManagerBase *>(getModuleByPath("^.^.^.^.^.^.dcManager"));

		if (!initialized) {
		    currentRemainingMIs = MIs;
		    currentIteration = 1;
		}

		// Create a timer for delaying the execution of this application
		cMessage *waitToExecuteMsg = new cMessage (Timer_WaitToExecute.c_str());
		selfMessage = waitToExecuteMsg;
		scheduleAt (simTime().dbl()+startDelay, waitToExecuteMsg);

    par ("initialized") = true;
}

void LocalApplication::finish(){

    if (selfMessage!=nullptr) {
        cancelEvent(selfMessage);
        selfMessage=nullptr;
    }


	// Finish the super-class
    UserAppBase::finish();



//    sendAbortRequest();
}

void LocalApplication::processSelfMessage (cMessage *msg){

    // Start execution?
	if (!strcmp(msg->getName(), Timer_WaitToExecute.c_str())){
        selfMessage=nullptr;
        delete (msg);
		// Delete msg!
	//TODO: No inicializar estos tiempos cuando ya estaban inicializados. Hace falta guardar el estado.
		// Starting time...
		simStartTime = simTime();
		runStartTime = time (nullptr);

		// Log (INFO)
		EV_INFO << "Starting execution! Current iteration:" << currentIteration << endl;
						
		// Init...		

		executeIORequest ();

	}
	else if (!strcmp(msg->getName(), IO_READ_OPERATION)){
        selfMessage=nullptr;
        delete (msg);
	    executeRead = false;
	    executeCPU = true;
	    endServiceIO = simTime();

	    total_service_IO += (endServiceIO - startServiceIO);

	    executeCPUrequest ();
	}
    else if (!strcmp(msg->getName(), IO_WRITE_OPERATION)){
        selfMessage=nullptr;
        delete (msg);
        executeWrite = false;
        executeRead = true;
        endServiceIO = simTime();

        total_service_IO += (endServiceIO - startServiceIO);

        // Is there is no error... continue app execution
        if (currentIteration < iterations){
            currentRemainingMIs = MIs;
            currentIteration++;
            executeIORequest ();
        } else {
            printResults();
            sendEndResponse();
        }
    }
	else {
	    selfMessage=nullptr;
	    delete (msg);
        error ("Unknown self message [%s]", msg->getName());
	}
}

void LocalApplication::processRequestMessage (SIMCAN_Message *sm){
    error ("This module cannot process request messages:%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
}

void LocalApplication::processResponseMessage (SIMCAN_Message *sm){

    SM_CPU_Message *sm_cpu;

		// Init...				
		sm_cpu = dynamic_cast<SM_CPU_Message *>(sm);
	
		// Response came from CPU system?
		if (sm_cpu != nullptr){

		    if (sm_cpu->getOperation() != SM_ExecCpu)           // and if the operation is unknown... raise an error!
	            error ("Unknown received message:%s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());


		    // Log (DEBUG)
		    EV_DEBUG << "(processResponseMessage) CPU Message received" << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
			
		    if (sm_cpu->getIsCompleted()) {
		        executeCPU = false;
		        executeWrite = true;

                // Get time!
                endServiceCPU = simTime ();

                // Increase total time for I/O
                total_service_CPU = total_service_CPU.dbl() + (endServiceCPU.dbl() - startServiceCPU.dbl());

                // Is there is no error... continue app execution
//                if (currentIteration < iterations){
////                    currentIteration++;
//                    currentRemainingMIs = MIs;
////                    executeCPUrequest ();
//                }
//                else{
//                    printResults();
//                    sendEndResponse();
//                }
                executeIORequest ();
		    } else {
		        currentRemainingMIs = sm_cpu->getMisToExecute();
		        EV_DEBUG << "APP Status - App: " << this->appInstance << " - RemainingMIs: " << currentRemainingMIs << " - Iterarion: " << currentIteration <<  endl;
		    }
			
			
            // Check operation... if OK, then delete the message!
			delete (sm);
		}

		// Wrong response message!
		else{
		}


}

void LocalApplication::executeIORequest(){
    // Create a timer for delaying the execution of this application
    startServiceIO = simTime();
    cMessage *waitToExecuteMsg = new cMessage (IO_READ_OPERATION);
    simtime_t operationTime = SimTime(0);

    if (executeRead) {
        EV_INFO << "Requesting read IO:" << inputDataSize << " MB" << endl;
        double diskReadBandwidth = getModuleByPath("^.^.^")->par("diskReadBandwidth");
        operationTime = SimTime(inputDataSize/diskReadBandwidth, SIMTIME_S);
    } else if (executeWrite) {
        EV_INFO << "Requesting write IO:" << outputDataSize << " MB" << endl;
        waitToExecuteMsg = new cMessage (IO_WRITE_OPERATION);
        double diskWriteBandwidth = getModuleByPath("^.^.^")->par("diskWriteBandwidth");
        operationTime = SimTime(outputDataSize/diskWriteBandwidth);
    }
    selfMessage=waitToExecuteMsg;
    scheduleAt (simTime() + operationTime, waitToExecuteMsg);
    // Log (INFO)

    // Execute the request!

}

void LocalApplication::executeCPUrequest(){

	// Reset timer!	
	startServiceCPU = simTime ();

	// Log (INFO)
	EV_INFO << "Requesting CPU:" << currentRemainingMIs << " MIs" << endl;

	// Execute the request!
	SIMCAN_request_cpu(currentRemainingMIs);
}

void LocalApplication::sendAbortRequest(){

    // Log (INFO)
    EV_INFO << "Aborting CPU execution" << endl;

    // Execute the request!
    SIMCAN_abort_request_cpu();
}

void LocalApplication::sendEndResponse(){

    // Log (INFO)
    EV_INFO << "App execution ended" << endl;

    pDataCentreManager->handleAppExecEndSingle(userInstance, vmInstance, appInstance, getIndex());
}

void LocalApplication::printResults (){

    double runtime;

        // Calculate the total runtime
        runEndTime = time (nullptr);
        runtime = difftime (runEndTime, runStartTime);

        // End time
        simEndTime = simTime();
        EV_INFO << "Execution results:" << endl;
        EV_INFO << " + Total simulation time (simulated):" << (simEndTime.dbl() - simStartTime.dbl()) << " seconds " << endl;
        EV_INFO << " + Total execution time (real):" <<  runtime << " seconds" << endl;
        EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << endl;
}

unsigned int LocalApplication::getCurrentIteration() const {
    return currentIteration;
}

void LocalApplication::setCurrentIteration(unsigned int currentIteration) {
    this->currentIteration = currentIteration;
}

unsigned int LocalApplication::getCurrentRemainingMIs() const {
    return currentRemainingMIs;
}

void LocalApplication::setCurrentRemainingMIs(
        unsigned int currentRemainingMIs) {
    this->currentRemainingMIs = currentRemainingMIs;
}

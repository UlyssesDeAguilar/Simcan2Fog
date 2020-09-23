#include "LocalApplication.h"
#define MAX_FILE_SIZE 2000000000

Define_Module(LocalApplication);

LocalApplication::~LocalApplication(){
}

void LocalApplication::initialize(){

		// Init the super-class
		UserAppBase::initialize();

		// App Module parameters
		inputDataSize  = (int) par ("inputDataSize") * MB;
		outputDataSize  = (int) par ("outputDataSize") * MB;
		MIs  = par ("MIs");
		iterations  = par ("iterations");
		currentIteration = 1;
		
		const char *newInputFile = par ("inputFile");
		inputFile = newInputFile;

		const char *newOutputFile = par ("outputFile");
		outputFile = newOutputFile;

		// Service times		
		total_service_IO = total_service_CPU = 0.0;
		startServiceIO = endServiceIO = 0.0;
		endServiceCPU = startServiceCPU = 0.0;
		readOffset = writeOffset = 0;
		
		// Boolean variables
		executeCPU = executeRead = executeWrite = false;

		// Create a timer for delaying the execution of this application
		cMessage *waitToExecuteMsg = new cMessage (Timer_WaitToExecute.c_str());
		scheduleAt (simTime().dbl()+startDelay, waitToExecuteMsg);
}

void LocalApplication::finish(){

	// Finish the super-class
    UserAppBase::finish();
}

void LocalApplication::processSelfMessage (cMessage *msg){

    // Start execution?
	if (!strcmp(msg->getName(), Timer_WaitToExecute.c_str())){
	
		// Delete msg!
		delete (msg);
	
		// Starting time...
		simStartTime = simTime();
		runStartTime = time (nullptr);

		// Log (INFO)
		EV_INFO << "Starting execution! Current iteration:" << currentIteration << endl;
						
		// Init...		
		startServiceIO = simTime();
		executeCPUrequest ();
	}
	else
		error ("Unknown self message [%s]", msg->getName());
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

		    // Log (DEBUG)
		    EV_DEBUG << "(processResponseMessage) CPU Message received" << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
			
			// Get time!
			endServiceCPU = simTime ();
				
			// Check operation... if OK, then delete the message!
			if (sm_cpu->getOperation() == SM_ExecCpu){
				delete (sm);
			}
			
			// and if the operation is unknown... raise an error!
			else{
				error ("Unknown CPU operation in the received CPU message:%s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
			}
			
			// Increase total time for I/O
			total_service_CPU = total_service_CPU.dbl() + (endServiceCPU.dbl() - startServiceCPU.dbl());
		}

		// Wrong response message!
		else{
			error ("Unknown received message:%s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
		}

		// Is there is no error... continue app execution
        if (currentIteration < iterations){
            currentIteration++;
            executeCPUrequest ();
        }
        else{
            printResults();
        }
}

void LocalApplication::executeCPUrequest(){

	// Reset timer!	
	startServiceCPU = simTime ();

	// Log (INFO)
	EV_INFO << "Requesting CPU:" << MIs << " MIs" << endl;

	// Execute the request!
	SIMCAN_request_cpu(MIs);
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



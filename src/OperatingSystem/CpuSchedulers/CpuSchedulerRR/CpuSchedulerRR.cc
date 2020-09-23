#include "CpuSchedulerRR.h"

Define_Module (CpuSchedulerRR);


CpuSchedulerRR::~CpuSchedulerRR(){
	requestsQueue.clear();			
}


void CpuSchedulerRR::initialize(){

    int i;

        // Init the super-class
        cSIMCAN_Core::initialize();

        // Init module parameters
        isVirtualHardware = par ("isVirtualHardware");
        numCpuCores = par ("numCpuCores");
        quantum = par ("quantum");

        // Init the cGates for Hypervisor
        fromHypervisorGate = gate ("fromHypervisor");
        toHypervisorGate = gate ("toHypervisor");

        // Init the cGates for checking Hub
        fromHubGate = gate ("fromHub");
        toHubGate = gate ("toHub");

        // Check connections
        if (!toHypervisorGate->getNextGate()->isConnected()){
            EV_ERROR << "toHypervisorGate is not connected";
            error ("toHypervisorGate is not connected");
        }

        // Init requests queue
        requestsQueue.clear();

        // Non-virtual hardware. Using all the cpu cores
        if (!isVirtualHardware){

            isRunning = true;

            managedCpuCores = numCpuCores;

            // State of CPUs
            isCPU_Idle = new bool [numCpuCores];

            // Init state to idle!
            for (i=0; i<numCpuCores; i++)
                isCPU_Idle[i] = true;

            // Index of each CPU core
            cpuCoreIndex = new unsigned int [numCpuCores];

            // Init state to idle!
            for (i=0; i<numCpuCores; i++)
                cpuCoreIndex[i] = i;
       }

       // Using virtual hardware.
       else{
           isRunning = false;
           isCPU_Idle = nullptr;
           cpuCoreIndex = nullptr;
           managedCpuCores = 0;
       }

        // Show additional data
        if (showInitValues){
            EV_INFO << "isRunning:" << std::boolalpha << isRunning << " - managedCpuCores:" << managedCpuCores << endl;
        }
}


void CpuSchedulerRR::finish(){

    // Finish the super-class
    cSIMCAN_Core::finish();
}


cGate* CpuSchedulerRR::getOutGate (cMessage *msg){

    cGate* outGate;

	    // Init...
        outGate = nullptr;

	    // If msg arrives from the Hypervisor
        if (msg->getArrivalGate()==fromHypervisorGate){
            outGate = toHypervisorGate;
        }

        // If msg arrives from the checking hub
        else if (msg->getArrivalGate()==fromHubGate){
            error ("This module cannot receive requests from the CPU system!");
        }

        // Msg arrives from an unknown gate
        else
            error ("Message received from an unknown gate [%s]", msg->getName());


	return outGate;
}


void CpuSchedulerRR::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void CpuSchedulerRR::processRequestMessage (SIMCAN_Message *sm){

	int cpuIndex;
	SM_CPU_Message *sm_cpu;

	 // Cast to CPU Message!
	sm_cpu = check_and_cast<SM_CPU_Message *>(sm);


	    // This scheduler is idle
	    if (!isRunning){
	        error ("Scheduler is not running and a request has been received.%s", sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
	    }

	    // Scheduler running...
	    else{

            EV_DEBUG << "(processRequestMessage) Processing request."<< endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

            // Check if this request has something to execute
            if ((!sm_cpu->getUseTime()) && (!sm_cpu->getUseMis())){
                error ("Empty CPU request.%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
            }

            // Valid CPU request
            else{

                // Set quantum
                sm_cpu->setQuantum(quantum);

                // Search for an empty cpu core
                cpuIndex = searchIdleCPU();

                // All CPUs are busy
                if (cpuIndex == SC_NotFound){

                    EV_DEBUG << "(processRequestMessage) No idle CPU found..." << endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
                    EV_INFO << "Pushing message to queue..." << endl;

                    // Enqueue current computing block
                    requestsQueue.insert (sm_cpu);
                }

                // At least, one cpu core is idle
                else{

                    // Set the CPU core for this request
                    sm_cpu->setNextModuleIndex(cpuIndex);

                    // Update state!
                    isCPU_Idle[cpuIndex]=false;

                    EV_DEBUG << "(processRequestMessage) CPU idle found..." << endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
                    EV_INFO << "Sending message to CPU["<< cpuIndex << "]" << endl;

                    // Send the request to the CPU processor
                    sendRequestMessage (sm_cpu, toHubGate);
                }
            }
	    }
}


void CpuSchedulerRR::processResponseMessage (SIMCAN_Message *sm){

	int cpuIndex;
	cMessage* unqueuedMessage;
	SIMCAN_Message *nextRequest;
	SM_CPU_Message *sm_cpu;
	SM_CPU_Message *sm_cpuNext;


	     // Cast
	     sm_cpu = check_and_cast<SM_CPU_Message *>(sm);


	    // Zombie request arrives. This scheduler has been disabled!
        if (!isRunning){
            EV_DEBUG << "(processResponseMessage) Zombie request arrived:" << endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
            delete(sm);
        }

        // Scheduler is running...
        else{

            // Update CPU state!
            cpuIndex = sm_cpu->getNextModuleIndex();

            // Check bounds
            if ((cpuIndex >= managedCpuCores) || (cpuIndex < 0))
                error ("\nCPU index error:%d. There are %d CPUs attached. %s\n", cpuIndex, managedCpuCores, sm_cpu->contentsToString(showMessageContents, showMessageTrace).c_str());
            else
                isCPU_Idle[cpuIndex] = true;


            EV_DEBUG << "(processResponseMessage) Arrives a message from CPU core:" << cpuIndex << endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;

            // Current computing block has not been completely executed
            if (!sm_cpu->getIsCompleted()){
                EV_INFO << "Request CPU not completed... Pushing to queue." << endl;
                sm_cpu->setIsResponse(false);
                requestsQueue.insert (sm_cpu);
            }

            // Current block has been completely executed
            else{
                EV_INFO << "Request CPU completed. Sending it back to the application." << endl;
                sm_cpu->setIsResponse(true);
                sendResponseMessage (sm_cpu);
            }


            // There are pending requests
            if (!requestsQueue.isEmpty()){

                // Pop
                unqueuedMessage = (cMessage *) requestsQueue.pop();

                // Dynamic cast!
                nextRequest = check_and_cast<SIMCAN_Message *>(unqueuedMessage);

                // Set the cpu core index
                nextRequest->setNextModuleIndex(cpuIndex);

                // Update state!
                isCPU_Idle[cpuIndex]=false;

                sm_cpuNext = check_and_cast<SM_CPU_Message *>(nextRequest);

                // Set quantum
                sm_cpuNext->setQuantum(quantum);

                // Debug
                EV_DEBUG << "(processResponseMessage) CPU idle found..." << endl << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << endl;
                EV_INFO << "Sending message to CPU["<< cpuIndex << "]" << endl;

                // Send!
                sendRequestMessage (nextRequest, toHubGate);
            }

            // Empty queue
            else{
                EV_DEBUG << "(processResponseMessage) Empty queue!" << endl;
            }
        }
}


int CpuSchedulerRR::searchIdleCPU (){
	
	int i;
	bool found;
	int result;

		// Init
		i = 0;
		found = false;

		// Search for an idle CPU
		while ((i<managedCpuCores) && (!found)){

			if (isCPU_Idle[i])
				found = true;
			else
				i++;
		}

		// Result
		if (found)
			result = i;
		else
			result = SC_NotFound;

	return result;
}


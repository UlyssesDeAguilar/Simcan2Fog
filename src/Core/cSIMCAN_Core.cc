#include "cSIMCAN_Core.h"


cSIMCAN_Core::~cSIMCAN_Core(){

    // Delete latency message
    cancelAndDelete (latencyMessage);
}


void cSIMCAN_Core::initialize(){

    // Init common attributes
    currentRequest = 0;
    latencyMessage = nullptr;
    queue.clear();

    // Init module parameters
    showInitValues = par ("showInitValues");
    showMessageContents = par ("showMessageContents");
    showMessageTrace = par ("showMessageTrace");
    debugSimcanCore = par ("debugSimcanCore");

    // Show the initial parameters
    if (showInitValues)
        EV_DEBUG << initialParametersToString() << endl;
}

void cSIMCAN_Core::handleMessage(cMessage *msg){

	SIMCAN_Message *sm;

		// If msg is a Self Message...
		if (msg->isSelfMessage())
			processSelfMessage (msg);

		// Not a self message...
		else{

			// Casting to SIMCAN_Message
			if ((sm = dynamic_cast<SIMCAN_Message *>(msg)) != nullptr){

				// Request message, upload message trace and send to destination module!
				if (!sm->getIsResponse()){

				    // Debug (Trace)
				    if (debugSimcanCore)
				        EV_TRACE << "(handleMessage): Updating trace and inserting in queue."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;
				
					// Update message trace
					updateMessageTrace (sm);

					// Insert into queue
					queue.insert (sm);

					// If not processing any request...
					processCurrentRequestMessage ();
				}

				// Response message!
				else{
					processResponseMessage (sm);
				}
			}
						
			// msg is not a SIMCAN Message!
			else
				error ("Unknown received message [%s]", msg->getName());
		}
}

void cSIMCAN_Core::finish (){
}

void cSIMCAN_Core::sendRequestMessage (SIMCAN_Message *sm, cGate* gate){

	// If trace is empty, add current module and request number
	if (sm->isTraceEmpty()){
	    sm->addNodeToTrace (getFullPath());
		updateMessageTrace (sm);
	}

	// Debug (Trace)
	if (debugSimcanCore)
	    EV_TRACE << "(sendRequestMessage): Sending request message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

	// Send the message!
	send (sm, gate);

	// Process next request!
	processCurrentRequestMessage ();
}

void cSIMCAN_Core::sendResponseMessage (SIMCAN_Message *sm){

    if (sm->getResult()==10008)
        EV_INFO << "Parar" << endl;

	int gateId;

		// Get the gateId to send back the message
		gateId = sm->getLastGateId ();

		// Removes the current module from trace...
		sm->removeLastModuleFromTrace ();

		// Debug (Trace)
        if (debugSimcanCore)
            EV_TRACE << "(sendResponseMessage): Sending response message. Last module removed from trace."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

		// Send back the message
		send (sm, gateId);

		// Process next request!
		processCurrentRequestMessage ();
}

void cSIMCAN_Core::updateMessageTrace (SIMCAN_Message *sm){

	int gateId;

        // Debug (Trace)
        if (debugSimcanCore)
            EV_TRACE << "(updateMessageTrace): Before updating the trace."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

		// If gate==nullptr... This may be the first module, there is no previous gate!
		if ((getOutGate (sm))==nullptr)
			gateId = SC_UnsetGateID;
		else
			gateId = getOutGate (sm)->getId();

		// Add the current module to trace
		sm->addModuleToTrace (getId(), gateId, currentRequest);

		// Set the update trace
		currentRequest++;

		// Debug (Trace)
        if (debugSimcanCore)
            EV_TRACE << "(updateMessageTrace): After updating the trace."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;
}

void cSIMCAN_Core::processCurrentRequestMessage (){

	SIMCAN_Message *sm;
	cMessage *unqueuedMessage;


		// There is no pending request!
		//if (!isPendingRequest()){

			// While exists enqueued requests
			if (!queue.isEmpty()){

				// Pop!
				unqueuedMessage = (cMessage *) queue.pop();

				// Dynamic cast!
				sm = check_and_cast<SIMCAN_Message *>(unqueuedMessage);

				// Debug (Trace)
                if (debugSimcanCore)
                    EV_TRACE << "(processCurrentRequestMessage): Pop message for processing."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

				// Process
				processRequestMessage (sm);
			}

			// Empty queue
			else{

			    // Debug (Trace)
                if (debugSimcanCore)
                    EV_TRACE << "(processCurrentRequestMessage): No pending requests and queue is empty"<< endl;
			}
//		}
//
//		// There is a pending request
//		else{
//
//		    // Debug (Trace)
//            if (debugSimcanCore)
//                EV_TRACE << "(processCurrentRequestMessage): There is a pending requests..."<< endl;
//		}
}

bool cSIMCAN_Core::isPendingRequest (){

	bool result;

		// Init...
		result = false;

		// Check if latencyMsg is not nullptr!!!
		if (latencyMessage!=nullptr){
			if (latencyMessage->isScheduled())
				result = true;
		}

	return result;
}

string cSIMCAN_Core::initialParametersToString (){

    std::ostringstream info, unit;
    unsigned int i, numParameters;

        // Init
        info << endl;
        info << "Showing main parameters of " << this->getFullPath() << endl;

        // Get number of parameters
        numParameters = getNumParams();

        // For each parameter
        for (i=0; i<numParameters; i++){

            // Get the parameter
            cPar& currentParameter = par (i);

            // Clear stream
            unit.str("");
            unit.clear();

            // Set the units
            if (currentParameter.getUnit() != nullptr){
                unit << " in " << currentParameter.getUnit();
            }

           // Build the output
           info << "\t+ " << currentParameter.getName() << " = " << currentParameter.str() << unit.str() << endl;
        }

    return info.str();
}

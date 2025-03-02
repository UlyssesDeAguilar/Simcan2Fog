#include "cSIMCAN_Core.h"

cSIMCAN_Core::~cSIMCAN_Core()
{
	// Delete latency message
	//cancelAndDelete(latencyMessage);
}

void cSIMCAN_Core::initialize(int stage)
{
	if (stage == 0)
		initialize();
}

void cSIMCAN_Core::initialize()
{
	// Init common attributes
	currentRequest = 0;
	latencyMessage = nullptr;
	queue.clear();

	// Init module parameters
	showInitValues = par("showInitValues");
	showMessageContents = par("showMessageContents");
	showMessageTrace = par("showMessageTrace");
	debugSimcanCore = par("debugSimcanCore");

	// Show the initial parameters
	if (showInitValues)
		EV_DEBUG << initialParametersToString() << "\n";
}

void cSIMCAN_Core::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		processSelfMessage(msg);
		return;
	}

	// Try casting into a SIMCAN_Message
	auto sm = dynamic_cast<SIMCAN_Message *>(msg);

	// msg is not a SIMCAN Message!
	if (sm == nullptr)
		error("Unknown received message [%s]", msg->getName());

	// Request message, upload message trace and send to destination module!
	if (!sm->isResponse())
	{

		// Debug (Trace)
		if (debugSimcanCore)
			EV_TRACE << "(handleMessage): Updating trace and inserting in queue." << "\n"
					 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";

		// Update message trace
		updateMessageTrace(sm);

		// Insert into queue
		queue.insert(sm);

		// If not processing any request...
		processCurrentRequestMessage();
	}
	// Response message!
	else
	{
		processResponseMessage(sm);
	}
}

void cSIMCAN_Core::finish()
{
}

void cSIMCAN_Core::sendRequestMessage(SIMCAN_Message *sm, int gateId)
{
	// If trace is empty, add current module and request number
	if (sm->isTraceEmpty())
	{
		sm->addNodeToTrace(getFullPath());
		updateMessageTrace(sm);
	}

	// Debug (Trace)
	if (debugSimcanCore)
		EV_TRACE << "(sendRequestMessage): Sending request message." << "\n"
				 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";

	// Send the message!
	send(sm, gateId);

	// Process next request!
	processCurrentRequestMessage();
}

void cSIMCAN_Core::sendResponseMessage(SIMCAN_Message *sm)
{
	// Stop condition -- Fix magic number ?
	if (sm->getResult() == 10008)
		EV_INFO << "Parar" << "\n";

	// Get the gateId to send back the message
	auto gateId = sm->getLastGateId();

	// Removes the current module from trace...
	sm->removeLastModuleFromTrace();

	// Debug (Trace)
	if (debugSimcanCore)
		EV_TRACE << "(sendResponseMessage): Sending response message. Last module removed from trace." << "\n"
				 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";

	// Send back the message
	send(sm, gateId);

	// Process next request!
	processCurrentRequestMessage();
}

void cSIMCAN_Core::updateMessageTrace(SIMCAN_Message *sm)
{

	int gateId;

	// Debug (Trace)
	if (debugSimcanCore)
		EV_TRACE << "(updateMessageTrace): Before updating the trace." << "\n"
				 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";

	cGate *selection = getOutGate(sm);

	// If gate==nullptr... This may be the first module, there is no previous gate!
	if (selection == nullptr)
		gateId = SC_UnsetGateID;
	else
		gateId = selection->getId();

	// Add the current module to trace
	sm->addModuleToTrace(getId(), gateId, currentRequest);

	// Set the update trace
	currentRequest++;

	// Debug (Trace)
	if (debugSimcanCore)
		EV_TRACE << "(updateMessageTrace): After updating the trace." << "\n"
				 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";
}

void cSIMCAN_Core::processCurrentRequestMessage()
{
	// There is no pending request!
	// if (!isPendingRequest()){

	// Empty queue
	if (queue.isEmpty())
	{
		// Debug (Trace)
		if (debugSimcanCore)
			EV_TRACE << "(processCurrentRequestMessage): No pending requests and queue is empty" << "\n";
		return;
	}

	// There's at least one pending request
	auto unqueuedMessage = (cMessage *)queue.pop();
	auto sm = check_and_cast<SIMCAN_Message *>(unqueuedMessage);

	// Debug (Trace)
	if (debugSimcanCore)
		EV_TRACE << "(processCurrentRequestMessage): Pop message for processing." << "\n"
				 << sm->contentsToString(showMessageContents, showMessageTrace) << "\n";

	// Process the request
	processRequestMessage(sm);

	//		}
	//
	//		// There is a pending request
	//		else{
	//
	//		    // Debug (Trace)
	//            if (debugSimcanCore)
	//                EV_TRACE << "(processCurrentRequestMessage): There is a pending requests..."<< "\n";
	//		}
}

bool cSIMCAN_Core::isPendingRequest()
{
	// Check if latencyMsg is not nullptr!!!
	if (latencyMessage == nullptr)
		return false;

	// Will be true if it is pending
	return latencyMessage->isScheduled();
}

std::string cSIMCAN_Core::initialParametersToString()
{
	std::ostringstream out;

	// Init
	out << "\n";
	out << "Showing main parameters of " << this->getFullPath() << "\n";

	// For each parameter
	for (int i = 0, numParameters = getNumParams(); i < numParameters; i++)
	{
		// Get the parameter
		cPar &currentParameter = par(i);
		
		out << "\t+ " << currentParameter.getName() << " = " << currentParameter.str();
		
		// Set the units
		if (currentParameter.getUnit() != nullptr)
		{
			out << " in " << currentParameter.getUnit();
		}

		out << "\n";
	}

	return out.str();
}

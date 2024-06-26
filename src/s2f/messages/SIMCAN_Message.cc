#include "../messages/SIMCAN_Message.h"

Register_Class(SIMCAN_Message);

void TraceComponent::operator=(const TraceComponent &other)
{
	moduleID = other.moduleID;
	gateID = other.gateID;
	reqSequence = other.reqSequence;
}

TraceComponent::TraceComponent(int newModule, int newGate) : moduleID(newModule),
															 gateID(newGate)
{
}

TraceComponent::TraceComponent(int newModule, int newGate, reqNum_t newReq) : moduleID(newModule),
																			  gateID(newGate)
{
	reqSequence.push_front(newReq);
}

SIMCAN_Message::~SIMCAN_Message()
{
	// For each node...
	for (int node = 0; node < trace.size(); node++)
	{
		// For each module
		for (int module = 0; module < trace[node].second.size(); module++)
		{
			// Removes all lists belonging to current module
			trace[node].second[module].reqSequence.clear();
		}

		// Removes current node vector
		trace[node].second.clear();
	}

	// Removes completely the trace!
	trace.clear();

	// Delete if it has an IpStack
	if (ipStack != nullptr)
		delete ipStack;
}

SIMCAN_Message::SIMCAN_Message(const char *name, int kind) : SIMCAN_Message_Base(name, kind)
{
	setByteLength(SM_BaseLength);
	setName("SIMCAN_Message");
	parentRequest = nullptr;
	ipStack = nullptr;
}

SIMCAN_Message::SIMCAN_Message(const SIMCAN_Message &other) : SIMCAN_Message_Base(other)
{
	copy(other);
}

SIMCAN_Message &SIMCAN_Message::operator=(const SIMCAN_Message &other)
{
	if (this==&other) return *this;
    SIMCAN_Message_Base::operator=(other);
	copy(other);
	return *this;
}

void SIMCAN_Message::copy(const SIMCAN_Message &other)
{
	// Copy the message trace
	setTraceArraySize(other.getTraceArraySize());

	for (int i = 0; i < other.trace.size(); i++)
		this->addNodeTrace(other.trace[i].first, other.trace[i].second);

	// Copy the parent request
	parentRequest = other.parentRequest;

	// Copy the ipStack (if present)
	if (other.ipStack != nullptr)
	    ipStack = new IpStack(*other.ipStack);
	else
	    ipStack = nullptr;

	/* FIXME Old code... should we keep this ?

	// Copy the control info, if exists!
	if (getControlInfo() != NULL)
	{
		controlOld = check_and_cast<TCPCommand *>(getControlInfo());
		controlNew = new TCPCommand();
		controlNew = controlOld->dup();
		newMessage->setControlInfo (controlNew);
	}
	*/
}

void SIMCAN_Message::addNewIp(inet::L3Address addr)
{
	// Defer allocation to when really needed
	if (ipStack == nullptr)
		ipStack = new IpStack();

	ipStack->push(addr);
}

void SIMCAN_Message::popIp()
{
	if (ipStack == nullptr || ipStack->empty())
		throw cRuntimeError("IpStack null or empty in message of type %s \n", getClassName());
	
	ipStack->pop();
}

inet::L3Address SIMCAN_Message::getNextIp()
{
	if (ipStack == nullptr || ipStack->empty())
		throw cRuntimeError("IpStack null or empty in message of type %s \n", getClassName());
	
	return ipStack->top();
}

void SIMCAN_Message::setTraceArraySize(size_t size)
{
	trace.reserve(size);
}

size_t SIMCAN_Message::getTraceArraySize() const
{
	return trace.size();
}

void SIMCAN_Message::addModuleToTrace(int module, int gate, reqNum_t currentRequest)
{

	struct TraceComponent currentModule(module, gate);

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	currentModule.reqSequence.push_back(currentRequest);

	// Update current module trace
	trace.back().second.push_back(currentModule);
}

void SIMCAN_Message::removeLastModuleFromTrace()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Removes last module from trace!
	trace.back().second.pop_back();
}

void SIMCAN_Message::addNodeToTrace(std::string node)
{

	std::vector<TraceComponent> currentModuleTrace;

	// Add a new node trace
	trace.push_back(make_pair(node, currentModuleTrace));
}

void SIMCAN_Message::removeLastNodeFromTrace()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Removes last node trace
	trace.pop_back();
}

void SIMCAN_Message::addRequestToTrace(int request)
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Add a request number to last module trace
	trace.back().second.back().reqSequence.push_back(request);
}

void SIMCAN_Message::removeLastRequestFromTrace()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Add a request number to last module trace
	trace.back().second.back().reqSequence.pop_back();
}

int SIMCAN_Message::getLastGateId()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Update current module trace
	return trace.back().second.back().gateID;
}

int SIMCAN_Message::getLastModuleId()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Update current module trace
	return trace.back().second.back().moduleID;
}

int SIMCAN_Message::getCurrentRequest()
{

	// Check if trace is empty!
	if (trace.empty())
		throw cRuntimeError("Trace is empty!");

	// Update current module trace
	return trace.back().second.back().reqSequence.back();
}

cMessage *SIMCAN_Message::getParentRequest() const
{
	return parentRequest;
}

void SIMCAN_Message::setParentRequest(cMessage *parent)
{
	parentRequest = parent;
}

std::string SIMCAN_Message::traceToString()
{

	std::ostringstream traceLine;
	int currentNodeTrace;
	int currentModuleTrace;
	std::list<reqNum_t>::iterator listIterator;

	// Clear the string buffer
	traceLine.str("");

	// Walk through node traces
	for (currentNodeTrace = 0; currentNodeTrace < trace.size(); currentNodeTrace++)
	{

		// If not first element, add a separator!
		if (currentNodeTrace != 0)
			traceLine << "|";

		// Get current hostName
		traceLine << trace[currentNodeTrace].first << "@";

		// Walk through module traces
		for (currentModuleTrace = 0; currentModuleTrace < trace[currentNodeTrace].second.size(); currentModuleTrace++)
		{

			// If not first element, add a separator!
			if (currentModuleTrace != 0)
				traceLine << "/";

			// Add module ID
			traceLine << trace[currentNodeTrace].second[currentModuleTrace].moduleID << ",";

			// Add gate ID
			traceLine << trace[currentNodeTrace].second[currentModuleTrace].gateID << ":";

			// Walk through request list
			for (listIterator = trace[currentNodeTrace].second[currentModuleTrace].reqSequence.begin();
				 listIterator != trace[currentNodeTrace].second[currentModuleTrace].reqSequence.end();
				 ++listIterator)
			{

				// If not the first element, add a separator!
				if (listIterator != trace[currentNodeTrace].second[currentModuleTrace].reqSequence.begin())
					traceLine << ".";

				traceLine << *listIterator;
			}
		}
	}

	return traceLine.str();
}

std::string SIMCAN_Message::contentsToString(bool showContents, bool includeTrace)
{

	std::ostringstream osStream;
	// TODO: TCPCommand *ind;

	// Init...
	osStream.str("");
	osStream.clear();

	if (showContents)
	{

		//		// Get control info
		//		if (getControlInfo() != NULL)
		//			ind = check_and_cast<TCPCommand *>(getControlInfo());
		//		else
		//			ind = NULL;

		osStream << "Message contents:" << endl;
		osStream << " + Name:" << getName() << " - Operation:" << operationToString(getOperation()) << " - Length:" << getByteLength() << " bytes" << endl;
		osStream << " + isResponse:" << std::boolalpha << isResponse() << " - RemoteOperation:" << std::boolalpha << getRemoteOperation() << " - NextModuleIndex:" << getNextModuleIndex() << endl;
		osStream << " + ConnectionId:" << getConnectionId() << " - CommunicationId:" << getCommId() << " - SourceProcess ID:" << getSourceId() << " - Result:" << getResult() << endl;

		//		if (ind != NULL)
		//			osStream  << " - ControlInfo->connId:" <<  ind->getConnId() << endl;
		//		else
		//			osStream  << " - ControlInfo->connId: NULL" << endl;

		if (includeTrace)
			osStream << " + Trace:" << traceToString() << endl;
	}

	return osStream.str();
}

std::string SIMCAN_Message::operationToString(unsigned int operation)
{

	std::string result;

	if (operation == SM_NullOperation)
		result = "Operation not set!";

	// TODO: Add the rest of the types
	//		else if (operation == SM_OPEN_FILE)
	//			result = "SM_OPEN_FILE";
	//
	//		else if (operation == SM_CLOSE_FILE)
	//			result = "SM_CLOSE_FILE";
	//
	//		else if (operation == SM_READ_FILE)
	//			result = "SM_READ_FILE";
	//
	//		else if (operation == SM_WRITE_FILE)
	//			result = "SM_WRITE_FILE";
	//
	//		else if (operation == SM_CREATE_FILE)
	//			result = "SM_CREATE_FILE";
	//
	//		else if (operation == SM_DELETE_FILE)
	//			result = "SM_DELETE_FILE";
	//
	else if (operation == SM_ExecCpu)
		result = "SM_ExecCpu";
	else if (operation == SM_VM_Req)
		result = "SM_VM_Req";
	//
	//		else if (operation == SM_CREATE_CONNECTION)
	//			result = "SM_CREATE_CONNECTION";
	//
	//		else if (operation == SM_LISTEN_CONNECTION)
	//			result = "SM_LISTEN_CONNECTION";
	//
	//		else if (operation == SM_SEND_DATA_NET)
	//			result = "SM_SEND_DATA_NET";
	//
	//		else if (operation == SM_MEM_ALLOCATE)
	//			result = "SM_MEM_ALLOCATE";
	//
	//		else if (operation == SM_MEM_RELEASE)
	//			result = "SM_MEM_RELEASE";
	//
	//		else if (operation == MPI_SEND)
	//		    result = "MPI_SEND";
	//
	//		else if (operation == MPI_RECV)
	//		    result = "MPI_RECV";
	//
	//
	//		else if (operation == MPI_NULL)
	//			result = "MPI_NULL";
	//
	//		else if (operation == MPI_ERROR)
	//			result = "MPI_ERROR";
	//
	//		else if (operation == MPI_BARRIER)
	//			result = "MPI_BARRIER";
	//
	//		else if (operation == MPI_BARRIER_ACK)
	//			result = "MPI_BARRIER_ACK";
	//
	//		else if (operation == MPI_PROCESS)
	//			result = "MPI_PROCESS";
	//
	//		else if (operation == MPI_METADATA)
	//			result = "MPI_METADATA";
	//
	//		else if (operation == MPI_METADATA_ACK)
	//			result = "MPI_METADATA_ACK";
	//
	//		else if (operation == MPI_DATA)
	//			result = "MPI_DATA";
	//
	//		else if (operation == MPI_DATA_ACK)
	//			result = "MPI_DATA_ACK";
	//
	//		else if (operation == MPI_RESULT)
	//			result = "MPI_RESULT";
	//
	//		else if (operation == MPI_RESULT_ACK)
	//			result = "MPI_RESULT_ACK";
	//
	//		else if (operation == MPI_BARRIER_ACK_AND_READ_DATA)
	//			result = "MPI_BARRIER_ACK_AND_READ_DATA";
	//
	//		else if (operation == MPI_PROCESS_AND_SEND_DATA_ACK)
	//			result = "MPI_PROCESS_AND_SEND_DATA_ACK";
	//
	//		else if (operation == MPI_SEND_RESULT_AND_READ_DATA)
	//				result = "MPI_SEND_RESULT_AND_READ_DATA";
	//
	//		else if (operation == MPI_SEND_RESULT_ACK_AND_READ_DATA)
	//				result = "MPI_SEND_RESULT_ACK_AND_READ_DATA";

	else
		result = "Unknown operation!";

	return result;
}

std::string SIMCAN_Message::operationToString()
{

	return operationToString(getOperation());
}

bool SIMCAN_Message::isTraceEmpty()
{
	return (trace.empty());
}

bool SIMCAN_Message::fitWithParent()
{

	bool fit;
	size_t dot, twoDots;

	SIMCAN_Message *parent;
	std::string childTrace;
	std::string parentTrace;

	// Init...
	fit = false;
	parent = check_and_cast<SIMCAN_Message *>(getParentRequest());

	childTrace = traceToString();
	parentTrace = parent->traceToString();

	// Search for last dot!
	dot = childTrace.find_last_of(".");
	twoDots = childTrace.find_last_of(":");

	// if found... remove last sequence number!
	if ((dot != std::string::npos) && (twoDots != std::string::npos))
	{

		// last dot behind ':'
		if (dot > twoDots)
		{

			childTrace = childTrace.substr(0, dot);

			if (childTrace.compare(parentTrace) == 0)
				fit = true;
		}
	}

	return fit;
}

bool SIMCAN_Message::fitWithParent(cMessage *parentMsg)
{

	bool fit;
	size_t dot, twoDots;

	SIMCAN_Message *parent;
	std::string childTrace;
	std::string parentTrace;

	// Init...
	fit = false;
	parent = check_and_cast<SIMCAN_Message *>(parentMsg);

	childTrace = traceToString();
	parentTrace = parent->traceToString();

	// Search for last dot!
	dot = childTrace.find_last_of(".");
	twoDots = childTrace.find_last_of(":");

	// if found... remove last sequence number!
	if ((dot != std::string::npos) && (twoDots != std::string::npos))
	{

		// last dot behind ':'
		if (dot > twoDots)
		{

			childTrace = childTrace.substr(0, dot);

			if (childTrace.compare(parentTrace) == 0)
				fit = true;
		}
	}

	return fit;
}

void SIMCAN_Message::addNodeTrace(std::string host, std::vector<TraceComponent> nodeTrace)
{

	std::vector<TraceComponent>::iterator component_it;
	std::list<reqNum_t>::iterator sequence_it;
	struct TraceComponent currentComponent(0, 0);
	std::vector<TraceComponent> currentNodeTrace;

	// Copy current node trace
	for (component_it = nodeTrace.begin(); component_it != nodeTrace.end(); component_it++)
	{

		currentComponent.moduleID = (*component_it).moduleID;
		currentComponent.gateID = (*component_it).gateID;
		currentComponent.reqSequence.clear();

		// Copy the sequence list
		for (sequence_it = (*component_it).reqSequence.begin();
			 sequence_it != (*component_it).reqSequence.end();
			 sequence_it++)
		{

			currentComponent.reqSequence.push_back(*sequence_it);
		}

		// Add current module to node trace
		currentNodeTrace.push_back(currentComponent);
	}

	// Add a new node trace
	trace.push_back(make_pair(host, currentNodeTrace));
}

std::string SIMCAN_Message::getInitialModuleName(int k)
{
	return trace[k].first;
}

std::vector<TraceComponent> SIMCAN_Message::getNodeTrace(int k)
{
	return trace[k].second;
}

TraceComponent const &SIMCAN_Message::getTrace(size_t k) const
{
	return trace[k].second.back();
}

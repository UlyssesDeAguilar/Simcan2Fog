#include "../messages/SM_CPU_Message.h"

Register_Class (SM_CPU_Message);



SM_CPU_Message::~SM_CPU_Message(){
}


SM_CPU_Message::SM_CPU_Message(const char *name, int kind): SM_CPU_Message_Base(name,kind){

	setByteLength (SM_BaseLength);
	setName ("SM_CPU_Message");
}


SM_CPU_Message::SM_CPU_Message(const SM_CPU_Message& other): SM_CPU_Message_Base(other.getName()){

	operator=(other);
	setByteLength (SM_BaseLength);
	setName ("SM_CPU_Message");
}


SM_CPU_Message& SM_CPU_Message::operator=(const SM_CPU_Message& other){

	SM_CPU_Message_Base::operator=(other);
	return *this;
}


SM_CPU_Message *SM_CPU_Message::dup() const{

	SM_CPU_Message *newMessage;
	// TODO: TCPCommand *controlNew;
	// TODO: TCPCommand *controlOld;
	int i;

		// Create a new message!
		newMessage = new SM_CPU_Message();

		// Base parameters...
		newMessage->setOperation (getOperation());
		newMessage->setIsResponse (isResponse());
		newMessage->setRemoteOperation (getRemoteOperation());
		newMessage->setConnectionId (getConnectionId());
		newMessage->setCommId (getCommId());
		newMessage->setSourceId (getSourceId());
		newMessage->setNextModuleIndex (getNextModuleIndex());
		newMessage->setResult (getResult());

		newMessage->setByteLength (getByteLength());
		newMessage->setParentRequest (getParentRequest());

		// SM_CPU_Message parameters...
		newMessage->setUseTime(getUseTime());
		newMessage->setCpuTime (getCpuTime());
		newMessage->setUseMis(getUseMis());
		newMessage->setMisToExecute(getMisToExecute());
		newMessage->setCpuPriority (getCpuPriority());
		newMessage->setQuantum (getQuantum());
		newMessage->setIsCompleted (isCompleted());

		// TODO: Add control info when inet is included!
//		// Copy the control info, if exists!
//		if (getControlInfo() != NULL){
////			controlOld = check_and_cast<TCPCommand *>(getControlInfo());
////			controlNew = new TCPCommand();
////			controlNew = controlOld->dup();
////			newMessage->setControlInfo (controlNew);
//		}
//
		// Reserve memory to trace!
		newMessage->setTraceArraySize (getTraceArraySize());

		// Copy trace!
		for (i=0; i<trace.size(); i++){
			newMessage->addNodeTrace (trace[i].first, trace[i].second);
		}

	return (newMessage);
}


void SM_CPU_Message::updateLength (){
	
    // Set the new size!
    setByteLength (SM_BaseLength);
}


void SM_CPU_Message::executeMIs (double numberMIs){

    // Update quantum
    if (quantum != SM_CpuInfiniteQuantum){
        quantum--;
    }
	
    // Execute request
	if ((getMisToExecute() - numberMIs) > 0.0)
	    setMisToExecute (getMisToExecute()- numberMIs);
	else{
	    setMisToExecute (0.0);
	    setIsCompleted(true);
	}
}


void SM_CPU_Message::executeTime (simtime_t executedTime){
	
	simtime_t remainingTime;
	
        remainingTime = getCpuTime();

        // Update quantum
        if (quantum != SM_CpuInfiniteQuantum){
            quantum--;
        }

        // Execute request
        if ((remainingTime - executedTime) > simtime_t::ZERO)
            setCpuTime (remainingTime - executedTime);
        else{
            setCpuTime (simtime_t::ZERO);
            setIsCompleted(true);
        }
}


std::string SM_CPU_Message::contentsToString (bool showContents, bool includeTrace){

	std::ostringstream osStream;

	    // Init...
        osStream.str("");
        osStream.clear();

        if (showContents){
            osStream << SIMCAN_Message::contentsToString(showContents, includeTrace);
            osStream << " + useTime:" << std::boolalpha << getUseTime() << " - cpuTime:" << getCpuTime() << endl;
            osStream << " + useMIs:" << std::boolalpha << getUseMis() << " - MIsToExecute:" << getMisToExecute() << endl;
            osStream << " + CPU priority:" << getCpuPriority() << " - quantum:" << getQuantum() << " - isCompleted:" << std::boolalpha << isCompleted() << endl;
        }

	return osStream.str();
}

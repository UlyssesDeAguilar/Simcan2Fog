#include "DummyApplication.h"

Define_Module(DummyApplication);

DummyApplication::~DummyApplication(){
}

void DummyApplication::initialize(){

		// Init the super-class
		UserAppBase::initialize();

		// App Module parameters
		const char *testParameterChr = par ("testParameter");
		testParameter = testParameterChr;
}

void DummyApplication::finish(){

	// Finish the super-class
    UserAppBase::finish();

}

void DummyApplication::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages");
}

void DummyApplication::processRequestMessage (SIMCAN_Message *sm){
    error ("This module cannot process request messages:%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
}

void DummyApplication::processResponseMessage (SIMCAN_Message *sm){
    delete sm;
}

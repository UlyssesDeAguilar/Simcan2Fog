#include "LocalApplication.h"
Define_Module(LocalApplication);

void LocalApplication::initialize()
{
	// Init the super-class
	AppBase::initialize();

	// Prepare callback
	setReturnCallback(this);

	// App Module parameters
	inputDataSize = (int)par("inputDataSize");
	outputDataSize = (int)par("outputDataSize");
	MIs = par("MIs");
	iterations = par("iterations");
	inputFile = par("inputFile").stdstringValue();
	outputFile = par("outputFile").stdstringValue();

	// Service times
	total_service_IO = total_service_CPU = 0.0;

	// pDataCentreManager = dynamic_cast<DataCentreManagerBase *>(getModuleByPath("^.^.^.^.^.^.dcManager"));

	bool initialized = par("initialized");
	if (!initialized)
	{
		currentRemainingMIs = MIs;
		currentIteration = 1;
	}

	// Record times
	simStartTime = simTime();
	runStartTime = time(nullptr);

	par("initialized") = true;
}

void LocalApplication::processSelfMessage(cMessage *msg)
{
	read(inputDataSize);
}

void LocalApplication::returnRead(simtime_t timeElapsed)
{
	// Add time
	total_service_IO += timeElapsed;
	execute(currentRemainingMIs);
}

void LocalApplication::returnExec(simtime_t timeElapsed, SM_CPU_Message *sm)
{
	// Add time
	total_service_CPU += timeElapsed;
	write(outputDataSize);
}

void LocalApplication::returnWrite(simtime_t timeElapsed)
{
	// Add time
	total_service_IO += timeElapsed;
	currentIteration++;
	
	if (currentIteration < iterations)
	{
		read(inputDataSize);
	}
	else
	{
		_exit();
	}
}

void LocalApplication::finish()
{
	// Calculate the total runtime
	double runtime = difftime(time(nullptr), runStartTime);

	// Log results
	EV_INFO << "Execution results:" << '\n';
	EV_INFO << " + Total simulation time (simulated):" << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
	EV_INFO << " + Total execution time (real):" << runtime << " seconds" << '\n';
	EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << '\n';

	// Finish the super-class
	AppBase::finish();
}
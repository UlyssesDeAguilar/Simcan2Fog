#ifndef __LOCAL_APPLICATION_H_
#define __LOCAL_APPLICATION_H_

#include <omnetpp.h>
#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "Applications/Base/UserAppBase/UserAppBase.h"

// Forward declaration
class DataCentreManagerBase;

/**
 * @class LocalApplication LocalApplication.h "LocalApplication.h"
 *
 * Example of a sequential application without traces.
 * This application alternates I/O operations with CPU.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @author Ulysses de Aguilar
 * @date 2009-03-13 (v1)
 * @date 29/01/2024 (v2)
 */
class LocalApplication : public UserAppBase, public UserAppBase::ICallback
{

protected:
	unsigned int inputDataSize;	 //!< Size of the data to be read (in bytes)
	unsigned int outputDataSize; //!< Size of the data to be written (in bytes)
	unsigned int MIs;			 //!< Number of MIs to be executed
	unsigned int iterations;	 //!< Number of iterations to be performed
	string inputFile;			 //!< Input file name
	string outputFile;			 //!< Output file name

	unsigned int currentRemainingMIs; //!< Number of MIs remaining to be executed
	unsigned int currentIteration;	  //!< Current iteration

	simtime_t simStartTime; //!< Simulation Starting timestamp
	time_t runStartTime;	//!< Running starting timestamp (real execution time)

	simtime_t total_service_CPU; //!< Spent time in CPU system
	simtime_t total_service_IO;	 //!< Spent time in IO system

	virtual ~LocalApplication() = default;
	virtual void initialize() override;
	virtual void finish() override;

	virtual void run() override;
	virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override;
	virtual void returnRead(simtime_t timeElapsed) override;
	virtual void returnWrite(simtime_t timeElapsed) override;

public:
	unsigned int getCurrentIteration() const { return currentIteration; }
	void setCurrentIteration(unsigned int currentIteration) { this->currentIteration = currentIteration; }
	unsigned int getCurrentRemainingMIs() const { return MIs; }
	void setCurrentRemainingMIs(unsigned int currentRemainingMIs) { this->MIs = MIs; }
};

#endif

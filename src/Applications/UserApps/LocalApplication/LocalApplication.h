#ifndef __LOCAL_APPLICATION_H_
#define __LOCAL_APPLICATION_H_

#include <omnetpp.h>
#include "Management/DataCenterManager/DataCenterManager.h"
#include "Applications/Base/UserAppBase/UserAppBase.h"

/**
 * @class LocalApplication LocalApplication.h "LocalApplication.h"
 *
 * Example of a sequential application without traces.
 * This application alternates I/O operations with CPU.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 */
class LocalApplication: public UserAppBase{
public:
    unsigned int getCurrentIteration() const;
    void setCurrentIteration(unsigned int currentIteration);
    unsigned int getCurrentRemainingMIs() const;
    void setCurrentRemainingMIs(unsigned int currentRemainingMIs);
    void sendAbortRequest();

	protected:

        /** Size of the data to be read (in bytes) */
		unsigned int inputDataSize;

		/** Size of the data to be written (in bytes) */
		unsigned int outputDataSize;

		/** Number of MIs to be executed */
		unsigned int MIs;
		unsigned int currentRemainingMIs;

		/** Number of iterations to be performed */
		unsigned int iterations;

		/** Current iteration */
		unsigned int currentIteration;

		/** Input file name */
		string inputFile;

		/** Output file name */
		string outputFile;

		/** Read offset */
        int readOffset;

        /** Write offset */
        int writeOffset;

		/** Simulation Starting timestamp */
		simtime_t simStartTime;

		/** Simulation Ending timestamp */
		simtime_t simEndTime;
		
		/** Running starting timestamp (real execution time) */
		time_t runStartTime;

		/** Running ending timestamp (real execution time) */
		time_t runEndTime;		
				
		/** Call Starting timestamp (IO) */
		simtime_t startServiceIO;
		
		/** Call Ending timestamp (IO) */
		simtime_t endServiceIO;
		
		/** Call Starting timestamp (CPU) */
		simtime_t startServiceCPU;
		
		/** Call Ending timestamp (CPU) */
		simtime_t endServiceCPU;
		
		/** Spent time in CPU system */
		simtime_t total_service_CPU;
		
		/** Spent time in IO system */
		simtime_t total_service_IO;
				
		/** Execute CPU */
		bool executeCPU;
		
		/** Execute read operation */
		bool executeRead;
				
		/** Execute write operation */
		bool executeWrite;
		
		DataCenterManager *pDataCenterManager;
				
								
		
	   /**
		* Destructor
		*/
		~LocalApplication();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize();

	   /**
 		* Module ending.
 		*/
	    virtual void finish();

	   /**
		* Process a self message.
		* @param msg Self message.
		*/
		void processSelfMessage (cMessage *msg);

	   /**
		* Process a request message.
		* @param sm Request message.
		*/
		void processRequestMessage (SIMCAN_Message *sm);

	   /**
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (SIMCAN_Message *sm);


		void sendEndResponse();


	private:

	   /**
		* Method that creates and sends a CPU request.
		*/
		void executeCPUrequest();

	   /**
		* Print results.
		*/
		void printResults();
};

#endif

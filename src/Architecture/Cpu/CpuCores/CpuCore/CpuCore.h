#ifndef __CPU_CORE_H_
#define __CPU_CORE_H_

#include "Core/cSIMCAN_Core.h"


/**
 * @class CpuCore CpuCore.h "CpuCore.h"
 *   
 * Network Service Module
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 */
class CpuCore : public cSIMCAN_Core{


	protected:

        /** Speed of the CPU core */
        double speed;

        /** Tick time (in seconds) */
        simtime_t tick;

        /** Instructions per tick */
        double ipt;
		
		/** Gate ID. Input gate. */
	    cGate* inGate;
	    
	    /** Gate ID. Output gate. */
	    cGate* outGate;  
	    
	    /** Pending message */
        cMessage *pendingMessage;

	        	
	   /**
	    * Destructor.
	    */    		
	    ~CpuCore();
	  	        			  	    	    
	   /**
	 	*  Module initialization.
	 	*/
	    void initialize() override;
	    
	   /**
	 	* Module ending.
	 	*/ 
	    void finish() override;
	    
	    
	private:
	
	   /**
		* Get the outGate ID to the module that sent <b>msg</b>
		* @param msg Arrived message.
		* @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
		*/ 
		cGate* getOutGate (cMessage *msg);

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
		
	   /**
		* Calculates the amount of time to execute completely the current computing block.
		* @param reainingMIs Million instructions to be executed.
		* @return Time to execute reainingMIs instructions.
		*/
		simtime_t getTimeToExecuteCompletely (unsigned long reainingMIs);
		
	   /**
		* Calculates the amount of time to execute completely the current computing block.
		* @param reainingTime Amount of time for executing current CB.
		* @return Amount of CPU time to execute reainingTime.
		*/
		simtime_t getMaximumTimeToExecute (simtime_t reainingTime);
};

#endif

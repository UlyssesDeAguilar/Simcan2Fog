#ifndef __SIMCAN_2_0_HARDWARE_REDIRECTOR_H_
#define __SIMCAN_2_0_HARDWARE_REDIRECTOR_H_

#include "Core/cSIMCAN_Core.h"


class Hypervisor :public cSIMCAN_Core{

	protected:

        /** Indicates if this module is able to virtualize hardware */
        bool isVirtualHardware;

        /** Maximum number of VMs allocated in this computer */
        unsigned int maxVMs;


        /** Input gate from Apps. */
        cGate** fromAppsGates;

        /** Output gate to Apps. */
        cGate** toAppsGates;

        /** Input gate from CPU. */
        cGate** fromCPUGates;

        /** Output gate to CPU. */
        cGate** toCPUGates;




//
//
//                                    /** Input gate from Memory. */
//                                    cGate* fromMemoryGate;
//
//                                    /** Input gate from Net. */
//                                    cGate* fromNetGate;

//                                    /** Input gate to Memory. */
//                                    cGate* toMemoryGate;
//
//                                    /** Input gate to Net. */
//                                    cGate* toNetGate;
		

	        	
	        	
	   /**
	    * Destructor.
	    */    		
	    ~Hypervisor();
	  	        			  	    	    
	   /**
	 	*  Module initialization.
	 	*/
	    void initialize();
	    
	   /**
	 	* Module ending.
	 	*/ 
	    void finish();
	    
	    
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
};

#endif

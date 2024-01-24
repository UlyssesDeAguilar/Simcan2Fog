#ifndef __DUMMY_APPLICATION_H_
#define __DUMMY_APPLICATION_H_

#include <omnetpp.h>
#include "Applications/Base/UserAppBase/UserAppBase.h"

/**
 * @class DummyApplication DummyApplication.h "DummyApplication.h"
 *
 * Dummy application.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 */
class DummyApplication: public UserAppBase{

	protected:

		/** Test parameter */
		string testParameter;
								
		
	   /**
		* Destructor
		*/
		~DummyApplication();

	   /**
 		*  Module initialization.
 		*/
	    virtual void initialize() override;

	   /**
 		* Module ending.
 		*/
	    virtual void finish() override;

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

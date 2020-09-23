#ifndef _SM_CPU_MESSAGE_H_
#define _SM_CPU_MESSAGE_H_

#include "SM_CPU_Message_m.h"


/**
 * @class SM_CPU_Message SM_CPU_Message.h "SM_CPU_Message.h"
 *
 * Class that represents a SM_CPU_Message.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 02-10-2007
 */
class SM_CPU_Message: public SM_CPU_Message_Base{


	public:

	   /**
		* Destructor.
		*/
		virtual ~SM_CPU_Message();

	   /**
		* Constructor of SIMCAN_Message
		* @param name Message name
		* @param kind Message kind
		*/
		SM_CPU_Message (const char *name=NULL, int kind=0);


	   /**
		* Constructor of SIMCAN_Message
		* @param other Message
		*/
		SM_CPU_Message(const SM_CPU_Message& other);


	   /**
		* = operator
		* @param other Message
		*/
		SM_CPU_Message& operator=(const SM_CPU_Message& other);


	   /**
		* Method that makes a copy of a SIMCAN_Message
		*/
		virtual SM_CPU_Message *dup() const;


	   /**
		* Update the message length
		*/
		void updateLength ();
		

	   /**
		* Update the MIs to be executed
		* @param numberMIs Number of instructions (measured in MIs) to be executed
		*/
		void executeMIs (double numberMIs);
   
	   /**
		* Update the amount of time to be exucuted
		* @param executedTime Amount of time for executing current CB
		*/
		void executeTime (simtime_t executedTime);

	   /**
		* Parse all parameters of current message to string.
		* @param showContents Shows the contents of the message.
		* @param includeTrace Message trace is also included in the parsing process.
		* @return String with the corresponding contents.
		*/
		virtual std::string contentsToString (bool showContents, bool includeTrace);
};

#endif

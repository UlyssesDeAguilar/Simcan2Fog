cplusplus {{

#include <list>
#include <string>
#include "s2f/core/include/SIMCAN_types.h"
	
   /**
	* Structure that represents a trace piece.
	*/
	struct TraceComponent{
		int moduleID;						/**< Module ID */
		int gateID;							/**< Gate ID to moduleID */
		std::list <reqNum_t> reqSequence;	/**< Requests sequence */		
		
	   /**
		* Assignment operator
		* @param other Trace component
		*/
		void operator=(const TraceComponent& other);	
		
	   /**
		* Constructor
		* @param newModule Module ID
		* @param newGate Gate ID		
		*/
		TraceComponent (int newModule, int newGate); 
		
	   /**
		* Constructor
		* @param newModule Module ID
		* @param newGate Gate ID
		* @param newReq Request number		
		*/
		TraceComponent (int newModule, int newGate, reqNum_t newReq);		
	};
}}

struct TraceComponent { @existingClass; }

//
// Base model for SIMCAN messages
//
// @author Pablo C. Cañizares
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
message SIMCAN_Message extends cPacket{
    @customize(true);
    unsigned int operation = SM_NullOperation; // Operation type
    bool isResponse = false; // Is a response message?
    bool remoteOperation = false; // Is a remote operation?	
	bool autoSourceTopic = true; // If set to true the push client will override the message SourceTopic
	string destinationTopic; // On which topic the request is pushed
	string returnTopic; // On which topic the return of the request should be pushed
    int connectionId = SM_UnsetValue; // Connection ID (Used in remote communication between nodes)
    int commId = SM_UnsetValue; // Communication ID (Used to locate a communication with server)
    int sourceId = SM_UnsetValue; // Source process ID
    int nextModuleIndex = SM_UnsetValue; // Index of the next destination gate (Used in requests messages).				
    int result = SM_UnsetValue; // Operation result.
    abstract TraceComponent trace[]; // Message trace			
}

cplusplus{{
	#include "s2f/messages/SIMCAN_Message.h"
}}

cplusplus {{
	#include "SIMCAN_Message.h"
	
}}

class SIMCAN_Message;

///
/// SIMCAN Message for User requests
///
message SM_UserAPP_Finish extends SIMCAN_Message{
    
    @customize(false);
           
    string userID;		// ID of the user instance
    string strVmId;	    // Id of the VM that allocs the APP
    string strApp;		// Name of the application
    int nTotalTime;		// Execution time of the application	
    //int nExecStatus;	// Execution status (idle, running, finished, timeout)
}




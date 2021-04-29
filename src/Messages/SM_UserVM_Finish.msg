cplusplus {{
	#include "SIMCAN_Message.h"
	
}}

class SIMCAN_Message;

///
/// SIMCAN Message for User requests
///
message SM_UserVM_Finish extends SIMCAN_Message{
    
    @customize(false);
           
    string userID;		// ID of the user instance
    string strVmId;	    // Id of the VM that allocs the APP    
    string strVmType;
}

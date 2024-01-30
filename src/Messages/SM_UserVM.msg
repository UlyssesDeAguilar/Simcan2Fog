import Messages.SIMCAN_Message;

cplusplus {{
#include "VMRequest.h"
}}

struct VM_Request { @existingClass; }

///
/// SIMCAN Message for User requests
///
message SM_UserVM extends SIMCAN_Message{
    
    @customize(true);
   
    double dStartSubscriptionTime;
    double dEndSubscriptionTime;     
    string strVmId;    
    string userID;			// ID of the user instance
    VM_Request vms[];    	// List of the Virtual machines
}

cplusplus{{
#include "Messages/SM_UserVM.h"
}}

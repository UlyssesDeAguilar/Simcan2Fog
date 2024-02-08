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
   
    double dStartSubscriptionTime;  // Start time of the whole vm deployment subscription (waiting in queue)
    double dEndSubscriptionTime;    // Finish time of the wait in queue
    string vmId;                    // Technically used for app timeouts ?
    string userId;			        // ID of the user instance
    abstract VM_Request vm[];    	// List of the Virtual machines
}

cplusplus{{
#include "Messages/SM_UserVM.h"
}}

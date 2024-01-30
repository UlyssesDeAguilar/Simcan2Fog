import Messages.SIMCAN_Message;

cplusplus {{
#include "AppRequest.h"
}}

struct APP_Request { @existingClass; }

///
/// SIMCAN Message for User requests
///
message SM_UserAPP extends SIMCAN_Message{
    
    @customize(true);
    
    string appId;    
    string vmId;    
    string userID;		// ID of the user instance
    abstract APP_Request app[];  // List of applications
    int nFinishedApps;	// Number of finished applications.  
    bool finished; 	// If the cloudprovide has already notified the user
}

cplusplus{{
#include "Messages/SM_UserAPP.h"
}}
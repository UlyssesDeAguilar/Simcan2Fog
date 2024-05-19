import Messages.SIMCAN_Message;

cplusplus {{
#include "AppRequest.h"
}}

struct AppRequest { @existingClass; }

///
/// SIMCAN Message for User requests
///
message SM_UserAPP extends SIMCAN_Message{
    @customize(true);
    //string ip;        -- RoutingInfo
    string vmId;    
    string userId;		         // ID of the user instance
    abstract AppRequest app[];   // List of applications
    int nFinishedApps;	         // Number of finished applications.  
    bool finished; 	             // If the cloudprovide has already notified the user
}

cplusplus{{
#include "Messages/SM_UserAPP.h"
}}

cplusplus {{
	#include "SIMCAN_Message.h"
	#include "SM_UserAPP_Finish_m.h"	
	
    struct APP_Request
	{
	    //request
		std::string strApp;		
		std::string strAppType;		
		std::string strIp;		
		std::string vmId;	
		int startTime;
		int finishTime;			
		tApplicationState eState;		
		SM_UserAPP_Finish* pMsgTimeout;
	};
}}

class SIMCAN_Message;
struct APP_Request;

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




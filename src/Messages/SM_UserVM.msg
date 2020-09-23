cplusplus {{
	#include "SIMCAN_Message.h"
	#include "SM_UserVM_Finish_m.h"
	
	struct VM_Response
	{
	    //response
	    int nOperationResult;
		double startTime;
		int nPrice;
		std::string strIp;		
	};
	
    struct VM_Request
	{
	    //request
		std::string strVmType;	
		std::string strVmId; 	
		int maxStartTime_t1;
		int nRentTime_t2;	
		int maxSubTime_t3;
		int maxSubscriptionTime_t4;
		
		SM_UserVM_Finish* pMsg; 
			
		//List of different options offered by the servers	
		std::vector <VM_Response> responseList; 
	};

}}

class SIMCAN_Message;
struct VM_Request;

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



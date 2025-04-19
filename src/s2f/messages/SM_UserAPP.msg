import s2f.messages.SIMCAN_Message;

cplusplus {{
    #include "s2f/core/include/SIMCAN_types.h"
}}

class AppRequest
{
    string name;                    //!< Name of the service being deployed
    string type;                    //!< Name of the instance
    double startTime = 0;           //!< Start of execution (in seconds)
    double finishTime = 0;          //!< End of execution   (in seconds)
    uint8_t state @customImpl;      //!< State of the application
    @str("App: " + name.str() + ", type:" + type.str() + ", current state: " + std::to_string(state) + ", starting time: " + std::to_string(startTime) + ", end time: " + std::to_string(finishTime));
};

cplusplus(AppRequest) {{
    bool isFinishedOK() { return state == appFinishedOK; }
    bool isFinishedKO() { return state == appFinishedError || state == appFinishedTimeout; }
    bool isFinished() { return isFinishedOK() || isFinishedKO(); }
}}

cplusplus(cc) {{
    void AppRequest::setState(uint8_t newState) 
    {
        state = newState;
        if (isFinished())
            finishTime = simTime().dbl();
    }
    uint8_t AppRequest::getState() const { return state; }
}}


///
/// SIMCAN Message for User requests
///
message SM_UserAPP extends SIMCAN_Message{
    @customize(true);
    operation = SM_APP_Req;
    string vmId;    
    string hypervisorUrl;
    string userId;		         // ID of the user instance
    AppRequest app[];            // List of applications
    int nFinishedApps = 0;	         // Number of finished applications.  
    bool finished = false; 	             // If the cloudprovide has already notified the user
}

cplusplus{{
#include "s2f/messages/SM_UserAPP.h"
}}
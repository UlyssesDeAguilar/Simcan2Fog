import s2f.messages.SIMCAN_Message;

cplusplus {{
#include "VMRequest.h"
}}

struct VM_Request { @existingClass; }

//
// Message for negotiating a VM deployment between the user and the CloudProvider
// 
// @author Pablo C. Cañizares
// @author Ulysses de Aguilar
// @version 2.0
message SM_UserVM extends SIMCAN_Message
{
    @customize(true);
    double dStartSubscriptionTime; // Start time of the whole vm deployment subscription (waiting in queue)
    double dEndSubscriptionTime; // Finish time of the wait in queue
    string vmId; // Technically used for app timeouts ?
    string userId; // ID of the user instance
    string zone; // Zone for the deployment
    string domain; // Domain where the services are deployed
    VM_Request vm[]; // List of the Virtual machines
}

cplusplus{{
#include "s2f/messages/SM_UserVM.h"
}}

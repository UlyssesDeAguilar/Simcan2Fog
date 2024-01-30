cplusplus{{
#include "../OperatingSystem/Hypervisors/common.h"
using hypervisor::Syscall;
using hypervisor::CallContext;
}};

import Messages.SIMCAN_Message;

class CallContext{@existingClass;}

message SM_Syscall extends SIMCAN_Message {
    operation = SM_Syscall_Req;     // SIMCAN request type 
    int pid;                        // Process ID
    int vmId;                       // VM ID
    CallContext context;            // Context for the operation call
}
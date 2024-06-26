import s2f.messages.SIMCAN_Message;

cplusplus{{
#include "inet/networklayer/common/L3Address.h"
using namespace inet;
}}

class L3Address {
    @existingClass;
    @descriptor(false);
}

message SM_ResolverRequest extends SIMCAN_Message{
    string domainName;      // Requested URL
    L3Address resolvedIp;   // Retrieved IP     
}
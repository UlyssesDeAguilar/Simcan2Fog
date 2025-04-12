import s2f.messages.SIMCAN_Message;

// This message is sent when a VM renting extension is being negotiated
//  
message SM_VmExtend extends SIMCAN_Message
{
    operation = SM_ExtensionOffer;
    string userId;      // Owner of the vm
    string vmId;        // The vm we are talking about
    bool accepted;      // If the extension offer is accepted by the client (only makes sense as a request)
    int extensionTime;  // The time to extend the renting, in seconds
}

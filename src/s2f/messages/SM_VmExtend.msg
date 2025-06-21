import s2f.messages.SIMCAN_Message;

//
// Message for negotiating a VM extension between the user and the CloudProvider
// 
// @author Ulysses de Aguilar
// @version 1.0
message SM_VmExtend extends SIMCAN_Message
{
    operation = SM_ExtensionOffer; // SIMCAN operation code
    string userId; // Owner of the vm
    string vmId; // The global id of the vm
    string hypervisorUrl; // The hypervisorUrl, which identifies the hypervisor where the vm is allocated
    bool accepted; // If the extension offer is accepted by the client. Only makes sense when the message is a request
    int extensionTime; // The time to extend the renting, in seconds
}

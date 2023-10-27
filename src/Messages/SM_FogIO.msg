import Messages.SIMCAN_Message;

// This class encapsulates an IO request to a FogNode,
// let it be from the Cloud, another FogNode or an Edge device 
message SM_FogIO extends SIMCAN_Message{
    @customize(true);       // Enable customization of the class
    double requestSize;     // In MB
}
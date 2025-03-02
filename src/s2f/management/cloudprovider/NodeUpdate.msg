import s2f.messages.SIMCAN_Message;

message NodeUpdate extends SIMCAN_Message
{
    operation = SM_Node_Update;
    uint64_t availableCores;
    uint64_t availableRam;
    uint64_t availableDisk;
    string zone;
}

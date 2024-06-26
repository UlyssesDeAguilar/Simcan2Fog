import s2f.messages.SIMCAN_Message;

///
/// SIMCAN Message for User requests
///
message SM_CloudProvider_Control extends SIMCAN_Message{
    
    @customize(false);
        
    string messageText;		// ID of the user instance
}



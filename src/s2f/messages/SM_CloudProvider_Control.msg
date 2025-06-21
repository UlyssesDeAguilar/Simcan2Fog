import s2f.messages.SIMCAN_Message;

//
// Message for handling user timeouts
// #FIXME: Seems to not being used
//
// @author Pablo C. Cañizares
// @version 1.0
message SM_CloudProvider_Control extends SIMCAN_Message{
    @customize(false);
    string messageText;		// ID of the user instance
}

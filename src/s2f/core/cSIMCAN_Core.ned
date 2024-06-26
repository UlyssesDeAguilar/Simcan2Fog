package s2f.core;

//
// Base module that performs the basic operations for managing messages.
//
simple cSIMCAN_Core{
    
    parameters:
   		bool showInitValues = default (true);				// Show the values of the initial parameters and basic structures
   		bool showMessageContents = default (true);			// Show the content of each processed message in the log
   		bool showMessageTrace = default (false);			// Show the trace of each message, that is, its <b>trace</b> field (for deep-debugging purpose only)
   		bool debugSimcanCore = default (false);				// Show log messages of cSIMCAN_Core (for deep-debugging purpose only)
}
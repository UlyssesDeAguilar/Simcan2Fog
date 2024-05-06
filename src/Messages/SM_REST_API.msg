import Messages.SIMCAN_Message;

// This is the base message for app intercommunication across the model
message SM_REST_API extends SIMCAN_Message {
    //@customize(true);
    string url;         // Identifies the target service
    string payload;     // The payload (text encoded, probably should be JSON or XML)
}

cplusplus{{
	#include "Messages/SM_REST_API_m.h"
}}

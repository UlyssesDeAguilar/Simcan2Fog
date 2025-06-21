import s2f.messages.SIMCAN_Message;

cplusplus {{
    #include "s2f/core/include/SIMCAN_types.h"
}}

//
// Key/value struct for assigning parameters to applications
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
struct AppParameter
{
    string name; // Name of the parameter
    string value; // Value of the parameter
}


//
// Message for changing settings of the Applications
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message SM_AppSettings extends SIMCAN_Message
{
    operation = SM_App_Set;
    string vmId; // Id of the virtual machine where it's allocated
    string appName; // Name of the application in the deployment
    AppParameter parameters[]; // The parameters to insert/update for the app instance
}
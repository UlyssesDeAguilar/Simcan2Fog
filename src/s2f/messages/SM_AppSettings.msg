import s2f.messages.SIMCAN_Message;

cplusplus {{
    #include "s2f/core/include/SIMCAN_types.h"
}}

struct AppParameter
{
    string name;
    string value;
}

message SM_AppSettings extends SIMCAN_Message
{
    operation = SM_App_Set;
    string vmId;
    string appName;
    AppParameter parameters[];
}
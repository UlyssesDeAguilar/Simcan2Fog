cplusplus{{
#include "Core/include/SIMCAN_types.h"
}};

// This also generates a descriptor for parsim
class ServiceURL { @existingClass; }

class RoutingInfo extends cObject
{
    ServiceURL sourceUrl;
    ServiceURL destinationUrl;
}
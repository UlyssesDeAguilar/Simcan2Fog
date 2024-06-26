package s2f.core.simdata.mysql;
import s2f.core.simdata.SimSchema;

//
// This module loads and manages all the configuration options 
// for the simulation. Please if you use the paralellisation option
// make sure this module is available in all partitions
//
simple SimSchemaMySQL like SimSchema
{
    parameters:
        @class(simschema::SimSchemaMySQL);
        string hostName = default("");         // MySQL host
        string userName = default("sim");      // The user allowed for connection
}
package simcan2.Core.SimSchema.MySQL;
//
// This module loads and manages all the configuration options 
// for the simulation. Please if you use the paralellisation option
// make sure this module is available in all partitions
//
simple SimSchemaMySQL
{
    parameters:
        @class(SimSchemaMySQL);
        string hostName = default("");         // MySQL host
        string userName = default("sim");      // The user allowed for connection
}
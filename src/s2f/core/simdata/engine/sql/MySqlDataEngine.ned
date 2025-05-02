package s2f.core.simdata.engine.sql;
import s2f.core.simdata.engine.DataEngine;

//
// This module loads and manages all the configuration options 
// for the simulation. Please if you use the paralellisation option
// make sure this module is available in all partitions
//
simple MySqlDataEngine like DataEngine
{
    parameters:
        @class(s2f::data::MySqlDataEngine);
        string hostName = default("");         // MySQL host
        string userName = default("sim");      // The user allowed for connection
}
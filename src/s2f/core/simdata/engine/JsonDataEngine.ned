package s2f.core.simdata.engine;

//
// This module loads and manages all the configuration options 
// for the simulation based upon JSON-like definitions.
//
simple JsonDataEngine like DataEngine
{
    parameters:
        @class(s2f::data::JsonDataEngine);
        bool checkConsistency = default(true); // If true the data engine will check if the VMs referenced in SLAs exist in the provided definitions
        object apps;         // The JSON object defining the applications
        object vms;          // The JSON object defining the virtual machines
        object slas;         // The JSON object defining the service level agreements
        object users;        // The JSON object defining the users
        object experiments;  // The JSON object defining the experiments
}

package simcan2.Management.CloudProviders.CloudProviderBase;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Base module for Cloud Providers. This module parses and manages data-centers.
//
simple CloudProviderBase extends CloudManagerBase{
    
    parameters:    
        bool showDataCenters = default (true);				// Show the parsed Data-Centers?                	
        string dataCentersList;								// Configuration of the Data-Centers
           
    gates:  	    
        input fromDataCenter[];								// Input gates from Data centers  		
        output toDataCenter[];								// Output gates from Data centers
        
        input fromUserGenerator;							// Input gate from user generator module
        output toUserGenerator;								// Output gate to user generator module
}

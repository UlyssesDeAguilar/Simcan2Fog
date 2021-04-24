package simcan2.Management.CloudProviders.CloudProviderBase;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Base module for Cloud Providers. This module parses and manages data-centres.
//
simple CloudProviderBase extends CloudManagerBase{
    
    parameters:    
        bool showDataCentres = default (true);				// Show the parsed Data-Centres?                	
        string dataCentresList;								// Configuration of the Data-Centres
           
    gates:  	    
        input fromDataCentre[];								// Input gates from Data centres  		
        output toDataCentre[];								// Output gates from Data centres
        
        input fromUserGenerator;							// Input gate from user generator module
        output toUserGenerator;								// Output gate to user generator module
}

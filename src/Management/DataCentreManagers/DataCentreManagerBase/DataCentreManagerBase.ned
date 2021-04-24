package simcan2.Management.DataCentreManagers.DataCentreManagerBase;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Implementation of a Cloud DataCentre Manager
//
simple DataCentreManagerBase extends CloudManagerBase {   
   
       
    parameters:    
        bool showDataCentreConfig = default (true);			// Show the list of racks and nodes in the initilize stage                	
        string dataCentreConfig = default("");				// Configuration of this Data-Centre
		int cpuStatusInterval @unit(s)  = default(600s);
        
    //@class(DataCentreManagerBase); 
                 
    gates:
        input in;
        output out;
}

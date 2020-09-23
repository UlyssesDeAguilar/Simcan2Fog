package simcan2.Management.DataCenterManager;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Implementation of a Cloud DataCenter Manager
//
simple DataCenterManager extends CloudManagerBase {   
   
       
    parameters:    
        bool showDataCenterConfig = default (true);			// Show the list of racks and nodes in the initilize stage                	
        string dataCenterConfig = default("");				// Configuration of this Data-Center
        
    @class(DataCenterManager); 
                 
    gates:
        input in;
        output out;
}

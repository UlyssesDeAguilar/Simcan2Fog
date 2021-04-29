package simcan2.Management.DataCentreManagers.DataCentreManagerBase;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Implementation of a Cloud DataCentre Manager
//
simple DataCentreManagerBase extends CloudManagerBase {   
   
       
    parameters:    
        bool showDataCentreConfig = default (true);			// Show the list of racks and nodes in the initilize stage                	
        string dataCentreConfig = default("");				// Configuration of this Data-Centre
		int cpuStatusInterval @unit(s)  = default(3600s);   //
        int activeMachinesThreshold = default(0);           //
        bool forecastActiveMachines = default(false);       //
        int minActiveMachines = default(20);				//
        
    //@class(DataCentreManagerBase); 
                 
    gates:
        input in;
        output out;
}

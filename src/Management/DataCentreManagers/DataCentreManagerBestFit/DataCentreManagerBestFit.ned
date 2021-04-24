package simcan2.Management.DataCentreManagers.DataCentreManagerBestFit;
import simcan2.Management.DataCentreManagers.DataCentreManagerBase.DataCentreManagerBase;

//
// Implementation of a Cloud DataCentre Manager
//
simple DataCentreManagerBestFit extends DataCentreManagerBase { 
    parameters:    
    	bool checkReservedFirst = default (false);  
   @class(DataCentreManagerBestFit); 

}

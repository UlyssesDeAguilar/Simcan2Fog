package simcan2.Management.DataCenterManagers.DataCenterManagerBestFit;
import simcan2.Management.DataCenterManagers.DataCenterManager.DataCenterManager;

//
// Implementation of a Cloud DataCenter Manager
//
simple DataCenterManagerBestFit extends DataCenterManager { 
    parameters:    
    	bool checkReservedFirst = default (false);  
   @class(DataCenterManagerBestFit); 

}

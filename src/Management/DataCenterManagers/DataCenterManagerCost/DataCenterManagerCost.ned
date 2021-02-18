package simcan2.Management.DataCenterManagers.DataCenterManagerCost;
import simcan2.Management.DataCenterManagers.DataCenterManager.DataCenterManager;

//
// Implementation of a Cloud DataCenter Manager
//
simple DataCenterManagerCost extends DataCenterManager { 
    parameters:    
    	bool checkReservedFirst = default (false);  
   @class(DataCenterManagerCost); 

}

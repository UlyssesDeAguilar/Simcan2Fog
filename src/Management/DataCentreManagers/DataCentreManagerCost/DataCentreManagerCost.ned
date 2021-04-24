package simcan2.Management.DataCentreManagers.DataCentreManagerCost;
import simcan2.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;

//
// Implementation of a Cloud DataCentre Manager
//
simple DataCentreManagerCost extends DataCentreManagerFirstFit { 
    parameters:    
    	bool checkReservedFirst = default (false);  
   @class(DataCentreManagerCost); 

}

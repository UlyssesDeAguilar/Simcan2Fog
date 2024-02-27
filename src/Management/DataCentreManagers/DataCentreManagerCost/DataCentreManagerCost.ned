package simcan2.Management.DataCentreManagers.DataCentreManagerCost;
import simcan2.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;

//
// Implementation of a Cloud DataCentre Manager
//
simple DataCentreManagerCost extends DataCentreManagerFirstFit 
{ 
    parameters:    
        @class(DataCentreManagerCost);
        int reservedNodes = default(0);             // The number of reserved nodes
    	bool checkReservedFirst = default (false);  // Whether to check user priority when allocating vms or not
}

package s2f.management.managers;
import s2f.core.cSIMCAN_Core;

//
// Implementation of a Cloud DataCentre Manager
//
simple DcManager extends cSIMCAN_Core {
    parameters:    
        @class(DataCentreManagerBase);
        @display("i=block/classifier");
        bool showDataCentreConfig = default (true);			// Show the list of racks and nodes in the initialize stage  # DEPRECATED              	
        string dataCentreConfig = default("");				// Configuration of this Data-Centre
        string selectionStrategy = default("FirstFit");     // Node selection strategy, one of: FirstFit, BestFit, CostFit
        int reservedNodes = default(0);                     // The number of reserved nodes
        int minActiveMachines = default(20);				// The minimum number of active nodes
		int cpuStatusInterval @unit(s)  = default(3600s);   //
        int activeMachinesThreshold = default(0);           //
        bool forecastActiveMachines = default(false);       //
        
        bool hasCloudEvents = default(true);
    gates:
        input networkIn;
        output networkOut;
        inout localNetwork;
}

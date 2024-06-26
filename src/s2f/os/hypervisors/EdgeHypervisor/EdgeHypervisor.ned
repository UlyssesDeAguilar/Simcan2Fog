package s2f.OperatingSystem.Hypervisors.EdgeHypervisor;
import s2f.Core.cSIMCAN_Core;

simple EdgeHypervisor extends cSIMCAN_Core
{    
	parameters:
    	@class (hypervisor::EdgeHypervisor);
		bool active;				// If the hypervisor is currently active
	    bool isVirtualHardware;		// True if this computer allows virtualization and False in another case
		int maxVMs;					// Maximum number of VMs allocated in this computer 
    	int maxApps;				// Maximum number of Apps allocated per vm in this computer
		string apps = default("");	// The apps preloaded to the device
	gates:  	    
		inout networkComm;			// Input/Output from/to network (either local or the internet)
        input fromApps[];			// Input gates from Applications 
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector
        
        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
}

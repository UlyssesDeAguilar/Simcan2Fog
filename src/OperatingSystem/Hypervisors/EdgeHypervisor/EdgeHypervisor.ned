package simcan2.OperatingSystem.Hypervisors.EdgeHypervisor;
import simcan2.Core.cSIMCAN_Core;

simple EdgeHypervisor extends cSIMCAN_Core
{    
	parameters:
	    int maxApps;
    	@class (EdgeHypervisor);
    	
  	gates:  	    
		input fromNetwork;		// Input gate from network
        input fromApps;			// Input gate from Applications
        input fromCpuScheduler;	// Input gate from CPU schedulers
        
		output toNetwork;		// Output gate to network
        output toApps;			// Output gate to app vector
        output toCpuScheduler;	// Output gate to scheduler vector
}

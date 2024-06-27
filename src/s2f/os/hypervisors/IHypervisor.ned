package s2f.os.hypervisors;

// Interface that must be implemented by each hyervisor.
//
// The module that implements this interface virtualizes 
// the hardware of the computer where it is executed.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i> and <i>maxVMs=1</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
moduleinterface IHypervisor{
	parameters:
	    bool active;				// If the hypervisor is currently active
	    bool isVirtualHardware;		// True if this computer allows virtualization and False in another case
		//int maxVMs;					// Maximum number of VMs allocated in this computer 
		int maxApps;				// Maximum number of Apps allocated per vm in this computer
    
  	gates:  	    
		inout networkComm;			// Input/Output from/to network (either local or the internet)
		input fromDisk;
        input fromApps[];			// Input gates from Applications 
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector
        
        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
        output toDisk;
}

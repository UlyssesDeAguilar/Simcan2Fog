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
        string controlTablePath;
        string hardwareManagerPath;
        string dataManagerPath;
        string hostName;
        int maxAppsPerVm;
 
  	gates:  	    
		input fromNetwork;
		input fromDisk;
        input fromApps[];			// Input gates from Applications 
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector
        
        output toNetwork;
        output toDisk;
        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
}

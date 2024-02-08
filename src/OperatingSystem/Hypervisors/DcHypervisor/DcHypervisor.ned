package simcan2.OperatingSystem.Hypervisors.DcHypervisor;
import simcan2.OperatingSystem.Hypervisors.IHypervisor;
import simcan2.Core.cSIMCAN_Core;


// Module in charge of  virtualizing the hardware of the computer where it is executed.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i> and <i>maxVMs=1</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple DcHypervisor extends cSIMCAN_Core like IHypervisor{
    parameters:
        @class(hypervisor::DcHypervisor);
	    bool active;
	    bool isVirtualHardware;		// True if this computer allows virtualization and False in another case
		int maxVMs;					// Maximum number of VMs allocated in this computer 
    
  	gates:  	    
        input fromApps[];			// Input gates from Applications 
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector
        
        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
}

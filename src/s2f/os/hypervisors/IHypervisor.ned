package s2f.os.hypervisors;

//
// Interface that must be implemented by each hyervisor.
//
// The module that implements this interface virtualizes 
// the hardware of the computer where it is executed.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i> and <i>maxVMs=1</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-02-10
// @version 3.0
moduleinterface IHypervisor {
	parameters:
	    bool active @mutable; // If the hypervisor is currently active
	    bool isVirtualHardware; // True if this computer allows virtualization and False in another case
        string controlTablePath; // VmControlTable path
        string hardwareManagerPath; // HardwareManager path
        string dataManagerPath; // DataManager path
        string hostName; // The host name
        int maxAppsPerVm; // Maximum number of applications x vm
 
  	gates:  	    
		input fromNetwork; // Input gate from network
		input fromDisk; // Input gate from disk module
        input fromApps[]; // Input gates from Applications 
        input fromCpuScheduler[]; // Input gates from CPU schedulers vector
        
        output toNetwork; // Output gate to network
        output toDisk; // Output gate to disk module
        output toApps[]; // Output gates to Applications
        output toCpuScheduler[]; // Output gates to CPU schedulers vector
}

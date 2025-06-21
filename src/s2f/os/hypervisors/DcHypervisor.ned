package s2f.os.hypervisors;
import s2f.core.cSIMCAN_Core;

//
// Particularization of the hypervisor into a data centre environment. Multiple VMs,
// schedulers and applications are assumed. Also this module registers it's hostname/IP
// in the data centre network database.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i> and <i>maxVMs=1</i>.
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-02-10
// @version 2.0
simple DcHypervisor like IHypervisor
{
    parameters:
        bool active @mutable = default(false); // If the hypervisor is currently active
        int powerOnTime @unit(s) = default(60s); // Startup time
        bool isVirtualHardware;	// True if this computer allows virtualization and False in another case
        int maxVMs;	// Maximum number of VMs allocated in this computer 
		int maxApps; // Maximum number of Apps allocated per vm in this computer
        string netDbPath; // The datacentre network database path
        string hostName;  // The host name
        string controlTablePath; // VmControlTable path
        string hardwareManagerPath; // HardwareManager path
        string dataManagerPath; // DataManager path
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

package s2f.OperatingSystem.Hypervisors.DcHypervisor;
import s2f.OperatingSystem.Hypervisors.IHypervisor;
import s2f.Core.cSIMCAN_Core;


// Module in charge of  virtualizing the hardware of the computer where it is executed.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i> and <i>maxVMs=1</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple DcHypervisor extends cSIMCAN_Core like IHypervisor
{
    parameters:
        @class(hypervisor::DcHypervisor);
        bool active = default(false);
        bool isVirtualHardware;		// True if this computer allows virtualization and False in another case
        int maxVMs;					// Maximum number of VMs allocated in this computer 
		int maxApps;				// Maximum number of Apps allocated per vm in this computer
        int powerOnTime @unit(s) = default(60s);
    gates:
        inout networkComm;	        // Input/Output from/to network (either local or the internet)
        input fromDisk;
        input fromApps[];			// Input gates from Applications
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector

        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
        output toDisk;	
}

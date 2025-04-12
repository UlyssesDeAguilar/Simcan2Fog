package s2f.os.hypervisors;
import s2f.core.cSIMCAN_Core;

simple EdgeHypervisor like IHypervisor
{
    parameters:
        @class(hypervisor::EdgeHypervisor);
        bool active = default(true);					// If the hypervisor is currently active
        bool isVirtualHardware = default(false);		// True if this computer allows virtualization and False in another case
        string controlTablePath;
        string hardwareManagerPath;
        string dataManagerPath;
        string hostName = default("localhost");
        int maxAppsPerVm;

        @display("i=hypervisor,,0");
    gates:
        input serialIn;
        input fromNetwork;
        input fromDisk;
        input fromApps[];			// Input gates from Applications 
        input fromCpuScheduler[];	// Input gates from CPU schedulers vector

        output serialOut;
        output toNetwork;
        output toDisk;
        output toApps[];			// Output gates to Applications
        output toCpuScheduler[];	// Output gates to CPU schedulers vector
}

package s2f.os.hypervisors;
import s2f.core.cSIMCAN_Core;

//
// Particularization of the hypervisor to a more simplistic
// kind of os (e.g a raspberry pi). Main management comands
// and events are sent through the serialIn and serialOut gates.
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-02-10
// @version 2.0
simple EdgeHypervisor like IHypervisor
{
    parameters:
        bool active @mutable = default(true); // If the hypervisor is currently active
	    bool isVirtualHardware = default(false); // True if this computer allows virtualization and False in another case
        string controlTablePath; // VmControlTable path
        string hardwareManagerPath; // HardwareManager path
        string dataManagerPath; // DataManager path
        string hostName = default("localhost"); // The host name
        int maxAppsPerVm; // Maximum number of applications x vm

        @display("i=hypervisor,,0");
    gates:
        input serialIn; // Input from serial (e.g keyboard of a user)
        input fromNetwork; // Input gate from network
		input fromDisk; // Input gate from disk module
        input fromApps[]; // Input gates from Applications 
        input fromCpuScheduler[]; // Input gates from CPU schedulers vector

        output serialOut; // Output gate to serial (e.g monitor of a user)
        output toNetwork; // Output gate to network
        output toDisk; // Output gate to disk module
        output toApps[]; // Output gates to Applications
        output toCpuScheduler[]; // Output gates to CPU schedulers vector
}

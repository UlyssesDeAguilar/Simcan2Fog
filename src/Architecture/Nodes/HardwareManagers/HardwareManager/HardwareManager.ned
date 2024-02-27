package simcan2.Architecture.Nodes.HardwareManagers.HardwareManager;
import simcan2.Architecture.Nodes.HardwareManagers.IHardwareManager;

//
// Module that manages the hardware resources of a computer
//
simple HardwareManager like IHardwareManager{
    
    parameters:
        @class(HardwareManager);
        int address;                        // Ip address of the node it manages
   		bool isVirtualHardware;				// True if this blade allows virtualization and False in another case
        int maxVMs;							// Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int numCpuCores;					// Number of CPUs	
        int maxUsers;						// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        double memorySize @unit(GB);
        double diskSize @unit(GB);
}

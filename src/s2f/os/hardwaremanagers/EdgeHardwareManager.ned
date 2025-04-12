package s2f.os.hardwaremanagers;
//
// Module that manages the hardware resources of a computer
//
simple EdgeHardwareManager like IHardwareManager{
    
    parameters:
        @class(EdgeHardwareManager);
   		bool isVirtualHardware;				// True if this blade allows virtualization and False in another case
        int maxVMs;							// Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int maxUsers;						// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int numCpuCores;					// Number of CPUs	
        double memorySize @unit(MiB);
        double diskSize @unit(MiB);
        int address = default(0);
}

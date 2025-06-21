package s2f.os.hardwaremanagers;

//
// Module that manages the hardware resources of an cloud node.
// For example: a node with a hypervisor (VMware's ESXi)
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple DcHardwareManager like IHardwareManager{
    
    parameters:
        @class(DcHardwareManager);
   		bool isVirtualHardware; // True if this blade allows virtualization and False in another case
        int maxVMs; // Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int maxUsers; // Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int numCpuCores; // Number of CPUs	
        double memorySize @unit(MiB); // Memory of the node  
        double diskSize @unit(MiB); // Disk size of the node
        string resourceManagerPath; // Path to the central resource manager
        string networkCardPath; // Path to the network card module
}

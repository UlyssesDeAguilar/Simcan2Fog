package s2f.os.hardwaremanagers;

//
// Module that manages the hardware resources of an edge node.
// For example: a computer, raspberry pi, ...
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple EdgeHardwareManager like IHardwareManager{
    
    parameters:
   		bool isVirtualHardware; // True if this blade allows virtualization and False in another case
        int maxVMs; // Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int maxUsers; // Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int numCpuCores; // Number of CPUs	
        double memorySize @unit(MiB); // The memory size of the node
        double diskSize @unit(MiB); // The disk size of the node
        int address = default(0); // The address of the node (L2 or IP)
}

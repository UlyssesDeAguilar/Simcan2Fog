package s2f.os.hardwaremanagers;

// Interface that must be implemented by each Hardware Manager.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-03-10
// @version 1.1 
moduleinterface IHardwareManager
{    
    parameters:
   		bool isVirtualHardware; // True if this blade allows virtualization and False in another case
        int maxVMs; // Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int numCpuCores; // Number of CPUs	
        int maxUsers; // Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        double memorySize @unit(MiB); // The memory of the node
        double diskSize @unit(MiB); // The disk of the node
}

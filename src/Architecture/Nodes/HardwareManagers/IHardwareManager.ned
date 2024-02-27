package simcan2.Architecture.Nodes.HardwareManagers;

// Interface that must be implemented by each Hardware Manager.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
moduleinterface IHardwareManager{
    
    parameters:
        int address;                        // Ip address of the node it manages
   		bool isVirtualHardware;				// True if this blade allows virtualization and False in another case
        int maxVMs;							// Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int numCpuCores;					// Number of CPUs	
        int maxUsers;						// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        double memorySize @unit(GB);
        double diskSize @unit(GB);
}

package simcan2.Architecture.Nodes.Cloud;
import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Nodes.HardwareManagers.IHardwareManager;
import simcan2.OperatingSystem.OperatingSystem;
import simcan2.Architecture.Cpu.CpuProcessor.CpuProcessor;

//
// Module that represents a blade. Generally, this module is used in racks, 
// where a set of blades are allocated in a node board and connected to the same switch
//
//	- isVirtualHardware = true?
//		+ numVMSs = 1..numCpuCores
//		+ maxUsers = numVMs
//		
//	- isVirtualHardware = false?
//		+ numVMSs = 1
//		+ maxUsers = 1..N
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module CloudBlade
{
    parameters:
        @labels(node,ethernet-node);
		string address;												// The address of the Blade inside a Board 
        int numApps = default(5);									// Number of applications in this vector
        bool staticAppAssignment;									// True if the assignment of applications is static or False in another case
        bool isVirtualHardware;										// True if this blade allows virtualization and False in another case
        int maxUsers;												// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int maxVMs;													// Maximum number of VMs allocated in this blade (For non-cloud environments, use maxVMs=1)
        int numCpuCores;											// Number of CPU cores  
        double memorySize @unit(GB);
        double diskSize @unit(GB);
        double diskReadBandwidth @unit(Mbps);
        double diskWriteBandwidth @unit(Mbps);


        string hardwareManagerType = default("HardwareManager");	// Hardware manager type
        string hypervisorType;                      				// Hypervisor type
        string cpuSchedulerType = default("CpuSchedulerRR");		// CPU Scheduler type
        string cpuCoreType = default("CpuCore");					// Type of the CPU cores

        // Forwarding types
        // osModule.hypervisor.typename = hypervisorType;

        // Forwarding parameters
        *.numCpuCores = numCpuCores;
        *.maxVMs = maxVMs;
        *.isVirtualHardware = isVirtualHardware;

        // Select function selects value from a list given an index (starts at 0)
        // Converting a boolean gives us 0/1 for false/true
        // This way we can enforce the policy of the class as a fail-safe for bad configurations!
        hardwareManager.maxUsers = select(int(isVirtualHardware), maxUsers, maxVMs);
        osModule.maxUsers = select(int(isVirtualHardware), maxUsers, maxVMs);

        @display("bgb=521,326,,black,3;bgi=circuit,t");

    gates:
        inout comm; // It's literally unconnected --> should go to NetworkStack ?

    submodules:

        cpu: CpuProcessor {
            parameters:
                // numCpuCores = numCpuCores;
                cpuCoreType = cpuCoreType;
                staticAppAssignment = staticAppAssignment;
                @display("p=416,144;i=cpu");
        }

        hardwareManager: <hardwareManagerType> like IHardwareManager {
            parameters:
                // isVirtualHardware = isVirtualHardware;
                // maxVMs = maxVMs;
                // maxUsers = maxUsers;
                // numCpuCores = numCpuCores;
                memorySize = memorySize;
                diskSize = diskSize;
                @display("p=84,144;i=hardwareConfig");
        }

        osModule: OperatingSystem {
            parameters:
                staticAppAssignment = staticAppAssignment;
                // isVirtualHardware = isVirtualHardware;
                // maxVMs = maxVMs;
                // numCpuCores = numCpuCores;
                cpuSchedulerType = cpuSchedulerType;
                @display("p=256,144;i=os");
        }

	
	// FIXME: When networkstack module is defined place it in here !
    connections allowunconnected:
        // Connections between Operating System and CPU
        osModule.toCpu --> ned.IdealChannel --> cpu.fromOs;
        osModule.fromCpu <-- ned.IdealChannel <-- cpu.toOs;
}


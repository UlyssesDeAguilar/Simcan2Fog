package simcan2.Architecture.Nodes.Cloud;
import simcan2.Architecture.Disk.IDiskManager;
import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Nodes.HardwareManagers.IHardwareManager;
import simcan2.OperatingSystem.OperatingSystem;
import simcan2.OperatingSystem.Hypervisors.DcHypervisor.DcHypervisor;
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
        int address;                                                // Local Ip
        bool staticAppAssignment;									// True if the assignment of applications is static or False in another case
        bool isVirtualHardware;										// True if this blade allows virtualization and False in another case
        int maxUsers;												// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int maxVMs;													// Maximum number of VMs allocated in this blade (For non-cloud environments, use maxVMs=1)
        int numCpuCores;											// Number of CPU cores  
        double memorySize @unit(GB);
        double diskSize @unit(GB);
        double diskReadBandwidth @unit(Mbps);
        double diskWriteBandwidth @unit(Mbps);
		int numApps = default(5);									// Number of applications in this vector
			

        string hardwareManagerType = default("HardwareManager");	// Hardware manager type
        string hypervisorType = default("DcHypervisor");            // Hypervisor type
        string cpuSchedulerType = default("CpuSchedulerRR");		// CPU Scheduler type
        string cpuCoreType = default("CpuCore");					// Type of the CPU cores

        // Forwarding types
        // osModule.hypervisor.typename = hypervisorType;

        // Forwarding parameters
        *.numCpuCores = numCpuCores;
        *.maxVMs = maxVMs;
        *.isVirtualHardware = isVirtualHardware;
        osModule.numApps = numApps;
        hardwareManager.address = address;
        disk.diskReadBandwidth = diskReadBandwidth;
        disk.diskWriteBandwidth = diskWriteBandwidth;

        // Select function selects value from a list given an index (starts at 0)
        // Converting a boolean gives us 0/1 for false/true
        // This way we can enforce the policy of the class as a fail-safe for bad configurations!
        hardwareManager.maxUsers = select(int(isVirtualHardware), maxUsers, maxVMs);
        osModule.maxUsers = select(int(isVirtualHardware), maxUsers, maxVMs);

        @display("bgb=521,326,,black,3;i=device/device");

    gates:
        inout comm;
        input socketIn[];
        output socketOut[];

    submodules:

        cpu: CpuProcessor {
            parameters:
                // numCpuCores = numCpuCores;
                cpuCoreType = cpuCoreType;
                staticAppAssignment = staticAppAssignment;
                @display("p=417,125;i=cpu");
        }

        hardwareManager: <hardwareManagerType> like IHardwareManager {
            parameters:
                // isVirtualHardware = isVirtualHardware;
                // maxVMs = maxVMs;
                // maxUsers = maxUsers;
                // numCpuCores = numCpuCores;
                memorySize = memorySize;
                diskSize = diskSize;
                @display("p=85,125;i=hardwareConfig");
        }

        osModule: OperatingSystem {
            parameters:
                staticAppAssignment = staticAppAssignment;
                // isVirtualHardware = isVirtualHardware;
                // maxVMs = maxVMs;
                // numCpuCores = numCpuCores;
                cpuSchedulerType = cpuSchedulerType;
                @display("p=257,125;i=os");
        }

        disk: <default("DiskManager")> like IDiskManager {
            @display("p=259,262;is=vl");
        }
    connections:
        // Connections between OS and CPU
        osModule.toCpu --> ned.IdealChannel --> cpu.fromOs;
        osModule.fromCpu <-- ned.IdealChannel <-- cpu.toOs;

        // Connections between OS and Disk
        osModule.toDisk --> disk.diskIn;
        osModule.fromDisk <-- disk.diskOut;
        
        for i=0..(maxUsers * numApps)-1 {
        	osModule.socketIn++ <-- {@display("ls=,0");} <-- socketIn++;
            osModule.socketOut++ --> {@display("ls=,0");} --> socketOut++;
        }

        // In/Out connection of the Operating System with the data centre network
        osModule.comm <--> comm;
}


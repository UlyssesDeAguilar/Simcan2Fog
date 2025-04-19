package s2f.architecture.nodes.cloud;
import s2f.architecture.disk.IDiskManager;
import s2f.os.hardwaremanagers.IHardwareManager;
import s2f.os.hardwaremanagers.DcHardwareManager;
import s2f.os.OperatingSystem;
import s2f.os.hypervisors.DcHypervisor;
import s2f.architecture.cpu.CpuProcessor;
import s2f.architecture.net.card.NetworkCard;

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
        int address = default(-1);                                  // Local Ip
        bool staticAppAssignment = default(true);				    // True if the assignment of applications is static or False in another case
        bool isVirtualHardware = default(true);						// True if this blade allows virtualization and False in another case
        int maxUsers;												// Maximum number of users allowed to launch applications (For cloud environments, maxUsers=maxVMs)
        int maxVMs;													// Maximum number of VMs allocated in this blade (For non-cloud environments, use maxVMs=1)
        int numCpuCores;											// Number of CPU cores  
        double memorySize @unit(GB);
        double diskSize @unit(GB);
        double diskReadBandwidth @unit(Mbps);
        double diskWriteBandwidth @unit(Mbps);
        int numApps = default(5);									// Number of applications in this vector


        string cpuSchedulerType = default("CpuSchedulerRR");		// CPU Scheduler type
        string cpuCoreType = default("CpuCore");					// Type of the CPU cores

        // Forwarding types
        // osModule.hypervisor.typename = hypervisorType;

        // Forwarding parameters
        *.numCpuCores = this.numCpuCores;
        *.maxVMs = this.maxVMs;
        *.isVirtualHardware = this.isVirtualHardware;
        **.staticAppAssignment = this.staticAppAssignment;
        osModule.numApps = this.numApps;
        osModule.hypervisor.hostName = fullName();

        cpu.cpuCoreType = this.cpuCoreType;
        netCard.L2Address = this.address;

        disk.numVms = this.maxVMs;
        disk.diskReadBandwidth = this.diskReadBandwidth;
        disk.diskWriteBandwidth = this.diskWriteBandwidth;

        // Select function selects value from a list given an index (starts at 0)
        // Converting a boolean gives us 0/1 for false/true
        // This way we can enforce the policy of the class as a fail-safe for bad configurations!
        hardwareManager.maxUsers = select(int(isVirtualHardware), this.maxUsers, this.maxVMs);
		hardwareManager.networkCardPath = "^.netCard";
        hardwareManager.memorySize = this.memorySize;
        hardwareManager.diskSize = this.diskSize;
        osModule.maxUsers = select(int(isVirtualHardware), this.maxUsers, this.maxVMs);
        @display("bgb=521,326,,black,3;i=device/device");

    gates:
        inout comm;
        input socketIn[];
        output socketOut[];

    submodules:

        cpu: CpuProcessor {
            parameters:
                @display("p=420,145;i=cpu");
        }

        hardwareManager: <default("DcHardwareManager")> like IHardwareManager {
            parameters:
                @display("p=96,145;i=hardwareConfig");
        }

        osModule: OperatingSystem {
            parameters:
                @display("p=259,145;i=os");
        }

        disk: <default("DiskManager")> like IDiskManager {
            @display("p=259,262;is=vl");
        }

        netCard: NetworkCard {
            @display("p=259,49");
        }
    connections:
        // Connections between OS and CPU
        osModule.toCpu --> ned.IdealChannel --> cpu.fromOs;
        osModule.fromCpu <-- ned.IdealChannel <-- cpu.toOs;

        // Connections between OS and Disk
        osModule.toDisk --> disk.diskIn;
        osModule.fromDisk <-- disk.diskOut;

        for i=0..(maxUsers * numApps)-1 {
            osModule.socketIn++ <-- { @display("ls=,0"); } <-- socketIn++;
            osModule.socketOut++ --> { @display("ls=,0"); } --> socketOut++;
        }

        // In/Out connection of the Operating System with the data centre network
        osModule.fromNetwork <-- netCard.cardOut;
        osModule.toNetwork --> netCard.cardIn;

        netCard.comm <--> comm;
}


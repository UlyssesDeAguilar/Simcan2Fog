package s2f.os;
import s2f.os.hypervisors.IHypervisor;
import s2f.os.schedulers.CpuSchedulersVector;
import s2f.os.control.VmControlTable;
import s2f.apps.AppsVector;

// Module that simulates the Operating System in a computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module OperatingSystem
{
    parameters:
        bool staticAppAssignment;										// True if the assignment of application is static or False in another case
        bool isVirtualHardware;											// True if this blade allows virtualization and False in another case
        int numApps = default(5);										// Number of applications in all vectors
        int maxVMs;														// Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int maxUsers;
        int numCpuCores;												// Number of CPU cores in the CPU processor

        *.maxVMs = this.maxVMs;
        *.isVirtualHardware = this.isVirtualHardware;
        hypervisor.maxApps = this.numApps;
        
        // App parameters
        appsVectors[*].numApps = this.numApps;
        *.staticAppAssignment = this.staticAppAssignment;


        // Hypervisor setup
        hypervisor.active = default(true);
        hypervisor.maxAppsPerVm = this.numApps;
        hypervisor.controlTablePath = "^.controlTable";
        hypervisor.hardwareManagerPath = "^.^.hardwareManager";
		controlTable.numVMs = this.maxVMs;
		controlTable.numAppsPerVm = this.numApps;
		
		cpuSchedVector.numSchedulers = this.maxVMs;
        cpuSchedVector.numCpuCores = this.numCpuCores;
        
        @display("bgb=532.53,430.68002");
    gates:
        input fromNetwork;
        input fromCpu;
        input fromDisk;
        input socketIn[];

        output toNetwork;
        output toCpu;
        output toDisk;
        output socketOut[];

    submodules:

        // Hypervisor module
        hypervisor: <default("DcHypervisor")> like IHypervisor {
            parameters:
                @display("p=251.23001,181.39001;i=hypervisor");
        }

        controlTable: VmControlTable {
            @display("p=251,315");
        }
        // Vector of CPU schedulers
        cpuSchedVector: CpuSchedulersVector {
            parameters:
                @display("p=423.89,181.39001;i=scheduler");
        }

        // Vector of "users" with their respective apps
        appsVectors[maxUsers]: AppsVector {
            parameters:
                vmId = index;
                @display("p=90.21001,181.39001,c,120");
        }

    connections:

        // Connections between Applications gate and Hypervisor
        for i=0..maxUsers-1 {
            appsVectors[i].toOS --> ned.IdealChannel --> hypervisor.fromApps++;
            appsVectors[i].fromOS <-- ned.IdealChannel <-- hypervisor.toApps++;
        }

        // Connections between Hypervisor and CPU schedulers
        for i=0..maxVMs-1 {
            hypervisor.toCpuScheduler++ --> ned.IdealChannel --> cpuSchedVector.fromHypervisor++;
            hypervisor.fromCpuScheduler++ <-- ned.IdealChannel <-- cpuSchedVector.toHypervisor++;
        }

        for i=0..maxUsers-1, for j=0..numApps-1 {
            appsVectors[i].socketIn++ <-- { @display("ls=,0"); } <-- socketIn++;
            appsVectors[i].socketOut++ --> { @display("ls=,0"); } --> socketOut++;
        }

        // Connection between the CPU schedulers vector and the CPU module
        cpuSchedVector.toCpu --> ned.IdealChannel --> toCpu;
        cpuSchedVector.fromCpu <-- ned.IdealChannel <-- fromCpu;

        // Connections to the disk manager
        hypervisor.toDisk --> toDisk;
        fromDisk --> hypervisor.fromDisk;

        // Connection in/out from network to hypervisor
        hypervisor.toNetwork --> toNetwork;
        hypervisor.fromNetwork <-- fromNetwork;
}

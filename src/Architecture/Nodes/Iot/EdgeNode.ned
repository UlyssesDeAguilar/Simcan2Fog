package simcan2.Architecture.Nodes.Iot;

import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Cpu.CpuProcessor.CpuProcessor;
import simcan2.OperatingSystem.Hypervisors.EdgeHypervisor.EdgeHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.ICpuScheduler;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulerRR.CpuSchedulerRR;
import simcan2.Architecture.Network.Stack.NetworkStack;
import simcan2.Applications.Base.AppsVector;

module EdgeNode
{
    parameters:
        bool staticAppAssignment = default(true);            // It has to do with the dependency of the CheckingHubModule
        int maxApps = default(10);                           // Maximum app instances allowed

        // Hardware specs
        int numCpuCores = default(4);                        // The cores of the CPU equipped in this node
        int cpuSpeed = default(60000);						 // The speed (in MIPS) of each Core
        int cpuTick @unit(s) = default(60s);				 // The interval between CPU ticks (in seconds)
        double memorySize @unit(GB) = default(16GB);
        double diskSize @unit(GB) = default(500GB);
        double diskReadBandwidth @unit(Mbps) = default(800Mbps);
        double diskWriteBandwidth @unit(Mbps) = default(800Mbps);
		
		// An edge hypervisor is always active
		hy.active = true;
		
        // Propagating values to submodules + configuration
        *.numCpuCores = numCpuCores;
        *.staticAppAssignment = staticAppAssignment;
        *.isVirtualHardware = false;
        
        // Setup of the cores
        cpu.cpuCoreType = "CpuCore";
        cpu.cpuCore[*].speed = cpuSpeed;
        cpu.cpuCore[*].tick = cpuTick;
		
		// Max applications in vector
        apps.numApps = maxApps;
		apps.vmId = 0;
		
		// Pass settings down to the hardware manager
        hwm.maxVMs = 1;
        hwm.maxUsers = 1;
        hwm.memorySize = memorySize;
        hwm.diskSize = diskSize;

        @display("bgb=463,275");
    gates:
        inout comm;

    submodules:
        ns: NetworkStack {
            @display("p=220,221");
        }

        hy: EdgeHypervisor {
            @display("p=220,127");
        }

        hwm: HardwareManager {
            address = 0;
        }

        cpu: CpuProcessor {
            @display("p=402,127");
        }

        scheduler: <default("CpuSchedulerRR")> like ICpuScheduler {
            @display("p=316,127");
        }

        apps: AppsVector {
            @display("p=93,127");
        }

    connections:
        // Connect the whole outer comms to the stack
        comm <--> ns.ethg++;

        // Connect hypervisor to scheduler
        hy.toCpuScheduler++ --> scheduler.fromHypervisor;
        hy.fromCpuScheduler++ <-- scheduler.toHypervisor;

        // Enable network comms
        hy.networkComm <--> ns.eth_comm;

        // Connect CPU - Scheduler
        scheduler.toHub --> cpu.fromOs;
        scheduler.fromHub <-- cpu.toOs;

        // Connect apps to hypervisor
        hy.toApps++ --> apps.fromOS;
        hy.fromApps++ <-- apps.toOS;
}

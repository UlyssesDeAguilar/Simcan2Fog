package simcan2.Architecture.Nodes.Iot;

import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Cpu.CpuProcessor.CpuProcessor;
import simcan2.OperatingSystem.Hypervisors.EdgeHypervisor.EdgeHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.ICpuScheduler;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulerRR.CpuSchedulerRR;
import simcan2.Architecture.Network.Adapter.NetworkAdapter;
import simcan2.Applications.Base.AppsVector;

module EdgeNode
{
    parameters:
        bool staticAppAssignment = default(true);            // It has to do with the dependency of the CheckingHubModule
        int maxApps = default(10);                           // Maximum app instances allowed

        // Hardware specs
        int numCpuCores = default(4);                        // The cores of the CPU equipped in this node
        double memorySize @unit(GB) = default(16GB);
       	double diskSize @unit(GB) = default(500GB);
        double diskReadBandwidth @unit(Mbps) = default(800Mbps);
        double diskWriteBandwidth @unit(Mbps) = default(800Mbps);
        
        // Propagating values to submodules + configuration
        *.numCpuCores = numCpuCores;
        *.staticAppAssignment = staticAppAssignment;
        *.isVirtualHardware = false;
        
        apps.numApps = maxApps;
        cpu.cpuCoreType = "CpuCore";
        
        hwm.memorySize = memorySize;
        hwm.diskSize = diskSize;        
        
        @display("bgb=463,275");
    gates:
        inout comm;
        
    submodules:
        na: NetworkAdapter {
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
        // Connect hypervisor to scheduler
        hy.toCpuScheduler++ --> scheduler.fromHypervisor;
        hy.fromCpuScheduler++ <-- scheduler.toHypervisor;

        // Enable network comms
        hy.networkComm <--> comm;

        // Connect CPU - Scheduler
        scheduler.toHub --> cpu.fromOs;
        scheduler.fromHub <-- cpu.toOs;

        // Connect apps to hypervisor
        hy.toApps++ --> apps.fromOS;
        hy.fromApps++ <-- apps.toOS;
}

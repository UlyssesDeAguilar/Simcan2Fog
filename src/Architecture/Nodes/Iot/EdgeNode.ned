package simcan2.Architecture.Nodes.Iot;
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
		double diskReadBandwidth @unit(Mbps)  = default(800Mbps);
		double diskWriteBandwidth @unit(Mbps) = default(800Mbps);
    submodules:
        na: NetworkAdapter;
        hy: EdgeHypervisor;

        // Set up the cpu with kind and number of cores
        cpu: CpuProcessor
        {
            numCpuCores = numCpuCores;
            cpuCoreType = "CpuCore";
            staticAppAssignment = staticAppAssignment;
        };
        
        // Let the scheduler know the number of cores and the non virtual environement
        scheduler: <default("CpuShcedulerRR")> like ICpuScheduler
        {
            numCpuCores = numCpuCores;
            isVirtualHardware = false;
        };

        // Prepare the slots for the apps
        apps: AppsVector
        {
            numApps = numApps;
            staticAppAssignment = staticAppAssignment;
        };

        connections:
            // Connect hypervisor to scheduler
            hy.toCpuScheduler --> scheduler.fromHypervisor;
            hy.fromCpuScheduler <-- scheduler.toHypervisor;
            
            // Enable network comms
            hy.toNetwork   --> na.moduleIn;
            hy.fromNetwork <-- na.moduleOut;

            // Connect CPU - Scheduler
            scheduler.toHub --> cpu.fromOs;
            scheduler.fromHub <-- cpu.toOs;

            // Connect apps to hypervisor
            hy.toApps   --> apps.fromOS;
            hy.fromApps <-- apps.toOS;
}
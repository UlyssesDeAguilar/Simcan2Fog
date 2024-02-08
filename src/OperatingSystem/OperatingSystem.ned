package simcan2.OperatingSystem;
import simcan2.OperatingSystem.Hypervisors.IHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulersVector;
import simcan2.Applications.Base.AppsVector;


// Module that simulates the Operating System in a computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module OperatingSystem
{
    parameters:
        bool staticAppAssignment;										// True if the assignment of application is static or False in another case
        bool isVirtualHardware;											// True if this blade allows virtualization and False in another case
        int maxVMs;														// Maximum number of VMs allocated in this blade (For non-cloud environments, use "1")
        int maxUsers;
        int numCpuCores;												// Number of CPU cores in the CPU processor

        string cpuSchedulerType;										// CPU Scheduler type
		*.maxVMs = maxVMs;
        *.isVirtualHardware = isVirtualHardware;

        @display("bgb=532.53,430.68002");
    gates:
        input fromCpu;				// Input gate from the CPU schedulers vector
        output toCpu;				// Output gate to the CPU schedulers vector


    submodules:

        // Hypervisor module
        hypervisor: <default("DcHypervisor")> like IHypervisor {
            parameters:
                @display("p=251.23001,181.39001;i=hypervisor");
            gates:
                fromCpuScheduler[maxVMs];
                toCpuScheduler[maxVMs];
                fromApps[sizeof(fromApps)];
                toApps[sizeof(toApps)];
        }

        // Vector of CPU schedulers
        cpuSchedVector: CpuSchedulersVector {

            parameters:
                staticAppAssignment = staticAppAssignment;
                numSchedulers = maxVMs;
                numCpuCores = numCpuCores;
                // isVirtualHardware = isVirtualHardware;
                schedulerType = cpuSchedulerType;
                @display("p=423.89,181.39001;i=scheduler");
        }

        // Vector of "users" with their respective apps
        appsVectors[maxUsers]: AppsVector {
            parameters:
                numApps = numApps;
                staticAppAssignment = staticAppAssignment;
                @display("p=90.21001,181.39001,c,120;i=user");
        }


    connections:

        // Connections between Applications gate and Hypervisor
        for i=0..maxUsers-1 {
            appsVectors[i].toOS --> ned.IdealChannel --> hypervisor.fromApps[i];
            appsVectors[i].fromOS <-- ned.IdealChannel <-- hypervisor.toApps[i];
        }

        // Connections between HardwareRedirector and CPU schedulers
        for i=0..maxVMs-1 {
            hypervisor.toCpuScheduler++ --> ned.IdealChannel --> cpuSchedVector.fromHypervisor++;
            hypervisor.fromCpuScheduler++ <-- ned.IdealChannel <-- cpuSchedVector.toHypervisor++;
        }

        // Connection between the CPU schedulers vector and the CPU module
        cpuSchedVector.toCpu --> ned.IdealChannel --> toCpu;
        cpuSchedVector.fromCpu <-- ned.IdealChannel <-- fromCpu;
}

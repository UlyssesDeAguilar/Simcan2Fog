package simcan2.OperatingSystem;
import simcan2.OperatingSystem.Hypervisors.IHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulersVector;

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
        int numCpuCores;												// Number of CPU cores in the CPU processor

        string hypervisorType;											// Hypervisor type
        string cpuSchedulerType;										// CPU Scheduler type


        @display("bgb=287,354");
    gates:
        input fromApps[];			// Input gates from the applications vectors       
        input fromCpu;				// Input gate from the CPU schedulers vector
        input in;					// Input gate from the network switch
        

        output toApps[];			// Output gate to the applications vectors       
        output toCpu;				// Output gate to the CPU schedulers vector
        output out;					// Output gate to the network switch


    submodules:

        // Hypervisor module
        hypervisor: <hypervisorType> like IHypervisor {

            parameters:
                maxVMs = maxVMs;
                isVirtualHardware = isVirtualHardware;

                @display("p=125,235;i=hypervisor");
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
                isVirtualHardware = isVirtualHardware;
                schedulerType = cpuSchedulerType;
                @display("p=125,96;i=scheduler");
        }


    connections:

        // Connections between Applications gate and Hypervisor
        for i=0..sizeof(fromApps)-1 {
            fromApps[i] --> ned.IdealChannel --> hypervisor.fromApps[i];
            hypervisor.toApps[i] --> ned.IdealChannel --> toApps[i];
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

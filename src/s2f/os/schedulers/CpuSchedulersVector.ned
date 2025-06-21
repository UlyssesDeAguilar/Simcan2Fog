package s2f.os.schedulers;
import s2f.architecture.net.hub.CheckingHubModule;

//
// Vector that contains <i>maxVMs</i> CPU schedulers.
//
// In order to work, this module uses: 
// - CheckingHubModule: In order to deliver to the right scheduler the messages
// - ICpuScheduler: The schedulers that define the scheduling policy
//
// An important thing to note is that all the schedulers have the same policy
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module CpuSchedulersVector
{
    parameters:
        @display("bgb=334,173");
        bool staticAppAssignment; // True if the assignment of application is static or False in another case
        bool isVirtualHardware;	// True if this computer allows virtualization and False in another case
        int numSchedulers; // Number of schedulers in this vector
        int numCpuCores; // Number of CPU cores in the CPU processor

    gates:
        input fromHypervisor[]; // Input gates from the Hypervisor
        input fromCpu; // Input gate from the CPU

        output toHypervisor[]; // Output gates to the Hypervisor
        output toCpu; // Output gate to the CPU

    submodules:
        cpuScheduler[numSchedulers]: <default("CpuSchedulerRR")> like ICpuScheduler {       
            parameters:
                isVirtualHardware = parent.isVirtualHardware;
                numCpuCores = parent.numCpuCores;
                @display("p=253,88,r,120;i=scheduler");
        }

        schedHub: CheckingHubModule {  
            parameters:
                staticAppAssignment = parent.staticAppAssignment;
                @display("p=82,88;i=device/hub");
        }

    connections:

        // Connections between the vector outside and each scheduler
        for i=0..numSchedulers-1 {
            fromHypervisor++ --> ned.IdealChannel --> cpuScheduler[i].fromHypervisor;
            toHypervisor++ <-- ned.IdealChannel <-- cpuScheduler[i].toHypervisor;
        }

        // Connections between the schedulers and the hub
        for i=0..numSchedulers-1 {
            schedHub.toInput++ --> ned.IdealChannel --> cpuScheduler[i].fromHub;
            schedHub.fromInput++ <-- ned.IdealChannel <-- cpuScheduler[i].toHub;
        }

	    // Connections between the hub and the CPU gate
        schedHub.toOutput++ --> ned.IdealChannel --> toCpu;
        schedHub.fromOutput++ <-- ned.IdealChannel <-- fromCpu;
}

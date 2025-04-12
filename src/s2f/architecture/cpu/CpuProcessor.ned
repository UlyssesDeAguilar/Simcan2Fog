package s2f.architecture.cpu;
import s2f.architecture.net.hub.CheckingHubModule;


//
// Module that contains <i>numCpuCores</i> CPU cores.
// These CPU cores simulates the execution, of the requests sent by the applications, to perform computing.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module CpuProcessor
{
    parameters:
        int numCpuCores;							// Number of CPU cores
        string cpuCoreType;							// Type of the CPU cores
        bool staticAppAssignment;					// True if the assignment of application is static or False in another case
        @display("i=cpu,bgb=499,209");
    gates:
        input fromOs;					// Input gate from the Operating System
        output toOs;  					// Output gate to the Operating System

    submodules:
        cpuCore[numCpuCores]: <cpuCoreType> like ICpuCore {		// CPU cores        
            @display("p=209,85,r,120;i=device/cpu");
        }
        cpuHub: CheckingHubModule {       								// Hub that manages incoming messages 

            parameters:
                staticAppAssignment = parent.staticAppAssignment;
                @display("p=209,156;i=device/hub");
        }

    connections:

        // Connections between the hub and the CPU processor gate
        cpuHub.toInput++ --> ned.IdealChannel --> toOs;
        cpuHub.fromInput++ <-- ned.IdealChannel <-- fromOs;

        // Connections between the Cpu cores and the hub
        for i=0..numCpuCores-1 {
            cpuHub.toOutput++ --> ned.IdealChannel --> cpuCore[i].in;
            cpuHub.fromOutput++ <-- ned.IdealChannel <-- cpuCore[i].out;
        }
}

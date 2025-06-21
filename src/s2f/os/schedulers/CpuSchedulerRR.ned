package s2f.os.schedulers;
import s2f.core.cSIMCAN_Core;

// Module that implements a Generic CPU Scheduler.
//
// CPU scheduler that implements a Round-Robin policy
//
// For non-cloud environments, use <i>isVirtualHardware=false</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-12-25
// @version 2.0
simple CpuSchedulerRR extends cSIMCAN_Core like ICpuScheduler {
    parameters:
        @class(CpuSchedulerRR);
        bool isVirtualHardware;	// True if this blade allows virtualization and False in another case
        int numCpuCores; // Number of CPU cores in the CPU processor
        int quantum = default (10); // Quantum (in ticks)        
        
    gates:
        input fromHypervisor; // Input gate from Hypervisor
        input fromHub; // Input gate from Hub
        output toHypervisor; // Output gate to Hypervisor
        output toHub; // Output gate to Hub					
}

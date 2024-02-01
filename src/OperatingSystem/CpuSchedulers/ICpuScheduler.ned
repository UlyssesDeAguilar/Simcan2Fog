package simcan2.OperatingSystem.CpuSchedulers;

// Interface that must be implemented by each CPU scheduler.
//
// The module that implements this interface is in charge of managing
// application requests to be executed in a CPU processor.
//
// The ~HardwareManager module assigns CPU cores, of the CPU processor, to 
// the existent CPU schedulers. Thus, each scheduler may be in charge of managing
// a different number of CPU cores, which depends of the configuration set
// in the ~HardwareManager module.
//
// For non-cloud environments, use <i>isVirtualHardware=false</i>.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
moduleinterface ICpuScheduler{
    
    parameters:
        @display("p=125,96;i=scheduler");
        bool isVirtualHardware;		// True if this blade allows virtualization and False in another case
        int numCpuCores;			// Number of CPU cores in the CPU processor

    gates:
        input fromHypervisor;		// Input gate from the Hypervisor
        input fromHub;				// Input gate from the Hub

        output toHypervisor;		// Output gate to the Hypervisor
        output toHub;				// Output gate to the Hub
}

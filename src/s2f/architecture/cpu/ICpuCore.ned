package s2f.architecture.cpu;

// Interface that must be implemented by each CPU core.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
moduleinterface ICpuCore{    
    
    parameters:
        double speed;			// CPU core speed (in MIPS)
        double tick @unit(s);	// Tick time (in seconds)

    gates:
        input in;		// Input gate
        output out;		// Output gate

}

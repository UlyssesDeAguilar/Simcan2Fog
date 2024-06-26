package s2f.architecture.cpu;
import s2f.core.cSIMCAN_Core;

// Module that simulates the execution of application requests to perform computing.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple CpuCore extends cSIMCAN_Core like ICpuCore{
    parameters:
        double speed;			// CPU core speed (in Mips)
        double tick @unit(s);	// Tick time (in seconds)
        @class(CpuCore);

    gates:
        input in;			// Input gate
        output out;			// Output gate
}

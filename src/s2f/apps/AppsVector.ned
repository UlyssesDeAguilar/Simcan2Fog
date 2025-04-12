package s2f.apps;
import s2f.core.logic.LogicDemuxHub;


// Vector that contains <i>numApps</i> application modules.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module AppsVector
{
    parameters:
        int vmId = default(-1);             // The virtual machine ID where this vector is contained
        int numApps;						// Number of applications in this vector

        @display("bgb=432.15,270.9,,,0;i=appsSet,,0");
    gates:
        input socketIn[];
        output socketOut[];
        input fromOS;						// Input gate from the Operating System
        output toOS;						// Output gate to the Operating System

    submodules:
        appModule[numApps]: UserAppModule {	// Applications launched by the user      
            pid = index;
            @display("p=266.6,135.45,m,5,1,1;i=appsSet");
        }

        logicHub: LogicDemuxHub {
            parameters:
                demuxClass = default("PidProcessDemux");
                @display("p=137.59999,135.45");
        }

    connections:

        // Connections between the Hub and the Operating System gate    
        fromOS --> logicHub.mainIn;
        logicHub.mainOut --> toOS;

        // Connections between the applications and the Hub
        for i=0..numApps-1 {
            logicHub.demuxOut++ --> appModule[i].fromHub;
            logicHub.demuxIn++ <-- appModule[i].toHub;
            socketIn[i] --> appModule[i].socketIn;
            socketOut[i] <-- appModule[i].socketOut;
        }
}

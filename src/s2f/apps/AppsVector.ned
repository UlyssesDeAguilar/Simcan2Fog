package s2f.apps;
import s2f.architecture.net.hub.CheckingHubModule;


// Vector that contains <i>numApps</i> application modules.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module AppsVector
{
    parameters:
        int vmId = default(-1);             // The virtual machine ID where this vector is contained
        int numApps;						// Number of applications in this vector
        bool staticAppAssignment;			// True if the assignment of application is static or False in another case
		
        @display("bgb=856.935,461.82748");
    gates:
        input socketIn[];
        output socketOut[];
        input fromOS;						// Input gate from the Operating System
        output toOS;						// Output gate to the Operating System

    submodules:
        appModule[numApps]: UserAppModule {	// Applications launched by the user      
            pid = index;
            @display("p=435.375,250.47499,m,5,1,1;i=appsSet");
        }

        appHub: CheckingHubModule {					// Hub that manages messages to be received by the corresponding applicationç

            parameters:
                staticAppAssignment = staticAppAssignment;

                @display("p=435.76498,84.4425;i=device/hub");
            gates:
                fromInput[numApps];			// Gates from each application
                toInput[numApps];			// Gates to each application
                fromOutput[1];   			// Gate from the Operating System
                toOutput[1];				// Gate to the Operating System
        }


    connections:

        // Connections between the Hub and the Operating System gate    
        fromOS --> ned.IdealChannel --> appHub.fromOutput++;
        appHub.toOutput++ --> ned.IdealChannel --> toOS;

        // Connections between the applications and the Hub
        for i=0..numApps-1 {
            appHub.toInput++ --> appModule[i].fromHub;
            appHub.fromInput++ <-- appModule[i].toHub;
        }
        
        for i=0..numApps-1{
        	socketIn[i] --> appModule[i].socketIn;
        	socketOut[i] <-- appModule[i].socketOut;
        }
}

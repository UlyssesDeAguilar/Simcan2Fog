package simcan2.Applications.Base;
import simcan2.Applications.UserApps.IUserApp;

// Module that contains a user application. The parameter <i>userAppType</i> defines the name of the application to be executed.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module UserAppModule
{
    parameters:
        int pid = default(-1);                					// PID of the application (within the vector)
        string userAppName = default("DummyApplication");		// Name of the application
        volatile bool defaultInitialized = default(true);
        @display("bgb=221,131");
    gates:
        input fromHub;			// Input gate from ModuleHub
        output toHub;			// Output gate to ModuleHub
        input socketIn;
        output socketOut;

    submodules:
        app: <userAppName> like IUserApp if defaultInitialized {	// Application
            @display("p=110,53;i=app");
        }
    connections:
        fromHub --> app.in;
        toHub <-- app.out;
        socketIn --> app.socketIn;
        socketOut <-- app.socketOut;
}

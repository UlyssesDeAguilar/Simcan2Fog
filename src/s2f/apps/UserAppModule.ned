package s2f.apps;

// Module that contains a user application. The parameter <i>userAppType</i> defines the name of the application to be executed.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
module UserAppModule
{
    parameters:
        int pid = default(-1);                					// PID of the application
        int vmId = default(0);                                  // VM ID of the application
        string resolverPath = default("");                      // If empty, implies no resolver available
        string userAppName = default("DummyApplication");		// Name of the application
        volatile bool defaultInitialized @mutable = default(true);
        @display("bgb=221,131");
    gates:
        input fromHub;			// Input gate from ModuleHub
        output toHub;			// Output gate to ModuleHub
        input socketIn;
        output socketOut;

    submodules:
        app: <userAppName> like IApp if defaultInitialized {	// Application
            @display("p=110,53;i=app");
        }
    connections:
        fromHub --> app.in;
        toHub <-- app.out;
        socketIn --> app.socketIn;
        socketOut <-- app.socketOut;
}

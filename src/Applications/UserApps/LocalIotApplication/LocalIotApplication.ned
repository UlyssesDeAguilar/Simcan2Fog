package simcan2.Applications.UserApps.LocalIotApplication;
import simcan2.Applications.Base.UserAppBase.UserAppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple LocalIotApplication extends UserAppBase{

    parameters:
        @class(LocalIotApplication);
        int processingMIs = default(100);				    // Number of MIs to execute in each iteration
        int listeningPort = default(1000);					// The port where it will listen (UDP)	
        string endpointName = default("");					// The endpoint service name 
        int endpointIp = default(0);						// The endpoint ip
        string actuators = default("");						// Ips of the actuators
}

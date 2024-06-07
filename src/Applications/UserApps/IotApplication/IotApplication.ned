package simcan2.Applications.UserApps.IotApplication;
import simcan2.Applications.Base.UserAppBase.UserAppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple IotApplication extends UserAppBase{
    parameters:
        @class(IotApplication);
        int processingMIs = default(10);				    // Number of MIs to execute in each iteration
        int listeningPort = default(1000);					// The port where it will listen (UDP)	
        int synthSize @unit(B) = default(40B);               // The payload that'll be sent

        bool serverClosesOnReturn = default(false);         // If the conexion should terminate once the response is back
        double replyDelay @unit(s) = default(1s);               // The time the server should take to process the request
        int replySize  @unit(B) = default(20B);              // The expected size of the reply
        
        int pingsThreshold = default(5);                    // The number of pings where it will start processing
}
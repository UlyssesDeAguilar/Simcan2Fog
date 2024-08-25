package s2f.apps.iot;
import s2f.apps.AppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple LocalIotApplication extends AppBase{
    parameters:
        @class(LocalIotApplication);
        int processingMIs = default(10);				    // Number of MIs to execute in each iteration
        int listeningPort = default(1000);					// The port where it will listen (UDP)	
        int pingsThreshold = default(5);                    // The number of pings where it will start processing
}

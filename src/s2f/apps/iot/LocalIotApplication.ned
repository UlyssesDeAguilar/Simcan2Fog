package s2f.apps.iot;
import s2f.apps.AppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple LocalIotApplication extends AppBase{
    parameters:
        @class(LocalIotApplication);
        string parentPath = default("^.^.^.^.");
        string actuatorPattern = default("lamp*");
        int processingMIs;				    // Number of MIs to execute in each iteration
        int listeningPort;					// The port where it will listen (UDP)	
        int pingsThreshold;                 // The number of pings where it will start processing
}

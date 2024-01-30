package simcan2.Applications.UserApps.LocalApplication;
import simcan2.Applications.Base.UserAppBase.UserAppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple LocalApplication extends UserAppBase{

    parameters:
        @class(LocalApplication);
        int inputDataSize @unit(MiB) = default (10MiB);		// Size of the data to be read in each iteration
        int outputDataSize @unit(MiB) = default (5MiB);		// Size of the data to be written in each iteration
        string inputFile = default ("/inputFile_0.dat");	// Input file
        string outputFile = default ("/outputFile_0.dat");	// Output file
        int MIs = default (100);							// Number of MIs to execute in each iteration
        int iterations = default (5);						// Number of iterations to be performed
        bool initialized = default(false);					// True after application is initialized the first time
}
